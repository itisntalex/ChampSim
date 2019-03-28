#include <cstring>
#
#include <algorithm>
#
#include <cache.h>
#
#include "hyperionpolicy.hh"
#include "../sampler/samplerset.hh"
#include "../helpers.hh"

using namespace Hyperion;
using namespace Hyperion::Predictor;

Perceptron::OutputType HyperionPolicy::_threshold = 0x0;

HyperionPolicy::HyperionPolicy (const Perceptron::FeaturePointerVector &features) :
  _perc(Perceptron(features)), _percPrefetch(Perceptron(features)), _samp(0x40, 0x12),
  _shadowCache (LLC_SET, LLC_WAY) {
  // Allocating a buffer for the PC histories. There's one history per CPU core.
  this->_pchistory = new ProgramCounterType* [NUM_CPUS];

  for (std::size_t i = 0; i < NUM_CPUS; i++) {
    this->_pchistory[i] = new ProgramCounterType[HYPERION_PC_HISTORY_LENGTH];
    std::memset(this->_pchistory[i], 0, sizeof(ProgramCounterType) * HYPERION_PC_HISTORY_LENGTH);
  }

  // Allocating a buffer for the RRPVs.
  this->_rrpv = new uint8_t* [LLC_SET];

  for (std::size_t i = 0; i < LLC_SET; i++) {
    this->_rrpv[i] = new uint8_t[LLC_SET];

    for (std::size_t j = 0; j < LLC_SET; j++) {
      this->_rrpv[i][j] = HYPERION_RRPV_MAX;
    }
  }

  // Initializing a few metrics.
  this->_metrics["PREDICTED_BYPASS"] = 0x0;
	this->_metrics["PREDICTED_NO_BYPASS"] = 0x0;
	this->_metrics["MISPREDICTED_BYPASS"] = 0x0;
	this->_metrics["PREDICTIONS"] = 0x0;
}

HyperionPolicy::~HyperionPolicy () {
  if (this->_pchistory != nullptr) {
    for (std::size_t i = 0; i < NUM_CPUS; i++) {
      delete[] this->_pchistory[i];
    }

    delete[] this->_pchistory;
  }

  if (this->_rrpv != nullptr) {
    for (std::size_t i = 0; i < LLC_SET; i++) {
      delete[] this->_rrpv[i];
    }

    delete[] this->_rrpv;
  }
}

void HyperionPolicy::updatePolicyState (uint32_t cpu, uint32_t set, uint32_t way, uint64_t full_addr, uint64_t ip, uint64_t victim_addr, uint32_t type, uint8_t hit) {
  static int32_t psel = 0;

  bool record = false;
  Perceptron::OutputType confValue, threshold;

  if (way >= LLC_WAY) {
    return;
  }

  // Updating the state of the policy prior to any other work.
  this->_currentPC = ip;
  this->_currentCoreIdx = cpu;
  this->_currentAddress = full_addr;
  this->_currentSet = set;
  this->_currentType = type;
  this->_hit = hit;


  /*
   * If the current set is a sampled one, we have to trigger some additional
   * udpates of the predictor.
   */
  if (this->_samp.isSampledSet(set)) {
    this->_samp.access (this);
  }

  confValue = this->_perc (this);

  if (type == PREFETCH) {
    confValue = this->_percPrefetch (this);
  }

  // std::cout << confValue << std::endl;

  if (confValue >= 0) {
    this->_rrpv [set][way] = HYPERION_RRPV_MAX;
  } else {
    this->_rrpv [set][way] = HYPERION_RRPV_MIN;

    if (!hit) {
      bool saturated = false;
      for (std::size_t i = 0; i < LLC_WAY; i++) {
        if (this->_rrpv[set][i] == HYPERION_RRPV_MAX - 1) {
          saturated = true;
        }
      }

      for (std::size_t i = 0; i < LLC_WAY; i++) {
        if (!saturated && this->_rrpv[set][i] < HYPERION_RRPV_MAX - 1) {
          this->_rrpv[set][i]++;
        }
      }
    }

    this->_rrpv [set][way] = HYPERION_RRPV_MIN;
  }

  bool miss = !hit, fake_llc_miss = false, bypassed = false, bypass_misprediction = false;
  std::vector<ShadowCache::CacheBlock>::iterator it_cache;
  std::list<ShadowCache::CacheBlock>::iterator it_history;

  if (miss) {
    bypassed = this->_shadowCache.markedBypassed (set, ShadowCache::CacheBlock (HYPERION_RRPV_MAX, full_addr));

    fake_llc_miss = !this->_shadowCache.isCached (set, ShadowCache::CacheBlock (HYPERION_RRPV_MAX, full_addr));

    bypass_misprediction = !fake_llc_miss & bypassed;

    if (bypass_misprediction) {
      this->_metrics["MISPREDICTED_BYPASS"]++;
    } else {

    }

  } else {
    ShadowCache::CacheBlock hitBlock = this->_shadowCache (set, way);

    this->_shadowCache.unmarkBypassed (set, hitBlock);

    if (confValue >= 0) {
      hitBlock.rrpv () = HYPERION_RRPV_MAX;
    } else {
      hitBlock.rrpv () = HYPERION_RRPV_MIN;
    }
  }

  // Should we record the current PC in the history?
  record |= (type == LOAD);
  record |= (type == RFO);
  record |= (type == WRITEBACK);
  record |= (type == PREFETCH);

  // We were asked to record the current PC into the history.
  if (record) {
    std::memmove(&(this->_pchistory[cpu][1]), &(this->_pchistory[cpu][0]), sizeof(ProgramCounterType) * (HYPERION_PC_HISTORY_LENGTH - 1));
    this->_pchistory[cpu][0] = ip;
  }
}

uint8_t HyperionPolicy::findVictim (uint32_t cpu, uint64_t instr_id, uint32_t set, const BLOCK *current_set, uint64_t ip, uint64_t full_addr, uint32_t type) {
  uint8_t max_rrip = 0,
          lru_victim = 0xFF;
  bool switch_to_lru = true;

  Hyperion::Sampler::SamplerBlock fakeBlock, evictedBlock;
  Hyperion::Sampler::SamplerSet sampledSet;
  Hyperion::Sampler::SamplerSet::SamplerBlockVectorIterator it;

  Perceptron::OutputType confValue;

  for (std::size_t i = 0; i < LLC_WAY; i++) {
    if (this->_rrpv[set][i] == HYPERION_RRPV_MAX) {
      lru_victim = i;

      // A proper victim has been found. No need to switch to loose LRU policy.
      switch_to_lru = false;
    }
  }

  if (switch_to_lru) {
    for (std::size_t i = 0; i < LLC_WAY; i++) {
      if (this->_rrpv[set][i] >= max_rrip) {
        max_rrip = this->_rrpv[set][i];
        lru_victim = i;
      }
    }
  }

  // ATTEMP: Trying to add some bypass optimization.
  if (type != WRITEBACK) {
    confValue = this->_perc (this);

    if (type == PREFETCH) {
      confValue = this->_percPrefetch (this);
    }

    if (confValue >= 56) {
      // We should mark this block as bypassed.
      this->_shadowCache.markBypassed (set, ShadowCache::CacheBlock (HYPERION_RRPV_MAX, full_addr) );

      // Accounting for this bypass prediction.
      this->_metrics["PREDICTED_BYPASS"]++;

      lru_victim = LLC_WAY;
    } else {
      this->_shadowCache.unmarkBypassed (set, ShadowCache::CacheBlock (HYPERION_RRPV_MAX, full_addr) );

      // Accounting for this no-bypass prediction.
      this->_metrics["PREDICTED_NO_BYPASS"]++;
    }
  }

  if (this->_samp.isSampledSet (set) && switch_to_lru && lru_victim != LLC_WAY) {
    sampledSet = this->_samp [this->_samp.translateSetIndex (set)];
    evictedBlock = sampledSet [lru_victim];

    if (evictedBlock.prefetched ()) {
      this->_percPrefetch.train (evictedBlock, evictedBlock.lastTrace (), Perceptron::LRUEviction);
    } else {
      this->_perc.train (evictedBlock, evictedBlock.lastTrace (), Perceptron::LRUEviction);
    }
  }

  /**
   * Whatever happens above, the fake cache always behave as if it always evicts
   * and inserts.
   */
   ShadowCache::CacheBlock& fakeVictim = this->_shadowCache.findVictim (set);

   // Modifying the associated address.
   fakeVictim.addr () = full_addr;

   confValue = this->_perc (this);

   if (type == PREFETCH) {
     confValue = this->_percPrefetch (this);
   }

   if (confValue >= 0) {
     fakeVictim.rrpv () = HYPERION_RRPV_MAX;
   } else {
     bool saturated = false;

     fakeVictim.rrpv () = HYPERION_RRPV_MIN;

     for (std::size_t i = 0; i < LLC_WAY; i++) {
       if (this->_shadowCache (set, i).rrpv () == HYPERION_RRPV_MAX - 1) {
         saturated = true;
       }
     }

     for (std::size_t i = 0; i < LLC_WAY; i++) {
       if (!saturated && this->_shadowCache (set, i).rrpv () < HYPERION_RRPV_MAX - 1) {
         this->_shadowCache (set, i).rrpv ()++;
       }
     }

     fakeVictim.rrpv () = HYPERION_RRPV_MIN;
   }

  return lru_victim;
}

void HyperionPolicy::finalStats () {
  double misprediction_rate = 0.0f;

  if (this->_metrics["PREDICTED_BYPASS"] > 0) {
    misprediction_rate = static_cast<double>(this->_metrics["MISPREDICTED_BYPASS"]) / static_cast<double>(this->_metrics["PREDICTED_BYPASS"]);
  }

  std::cout << "Bypass misprediction rate: " << misprediction_rate * 100.0f << "%"
            << ", predicted bypass " << this->_metrics["PREDICTED_BYPASS"]
            << ", predicted no bypass " << this->_metrics["PREDICTED_NO_BYPASS"]
            << ", mispredicted bypass " << this->_metrics["MISPREDICTED_BYPASS"]
            << std::endl;
}

Perceptron* HyperionPolicy::perceptron () {
  return &(this->_perc);
}

Perceptron* HyperionPolicy::perceptronPrefetch () {
  return &(this->_percPrefetch);
}

uint8_t HyperionPolicy::currentCoreIdx () const {
  return this->_currentCoreIdx;
}

HyperionPolicy::ProgramCounterType HyperionPolicy::currentPC () const {
  return this->_currentPC;
}

HyperionPolicy::AddressType HyperionPolicy::currentAddress () const {
  return this->_currentAddress;
}

std::size_t HyperionPolicy::currentSet () const {
  return this->_currentSet;
}

uint32_t HyperionPolicy::currentType () const {
  return this->_currentType;
}

bool HyperionPolicy::hit () const {
  return this->_hit;
}

HyperionPolicy::ProgramCounterType HyperionPolicy::pcHistoryElement (uint32_t cpu, uint8_t idx) const {
  if (cpu >= NUM_CPUS) {
    // TODO: Error handling!
  }

  if (idx >= HYPERION_PC_HISTORY_LENGTH) {
    // TODO: Error handling!
  }

  return this->_pchistory[cpu][idx];
}

Perceptron::OutputType HyperionPolicy::threshold () {
  return HyperionPolicy::_threshold;
}

void HyperionPolicy::setThreshold (Perceptron::OutputType v) {
  HyperionPolicy::_threshold = v;
}
