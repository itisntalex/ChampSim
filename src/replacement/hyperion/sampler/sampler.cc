#include <algorithm>
#include <random>
#
#include <cache.h>
#
#include "sampler.hh"
#include "../predictor/hyperionpolicy.hh"
#include "../features/basefeature.hh"
#include "../helpers.hh"

// #define HYPERION_RANDOM_SAMPLER

using namespace Hyperion::Sampler;

/**
 * @brief
 * @param sets
 * @param associativity
 */
Sampler::Sampler (std::size_t sets, std::size_t associativity) :
  _sets(sets, SamplerSet(associativity)), _setLUT(),
  _associativity (associativity) {
#ifdef HYPERION_RANDOM_SAMPLER
  std::size_t window_size = LLC_SET / sets,
              cache_set_idx = 0x0;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::binomial_distribution<> dis(window_size - 1, 0.5f);
#endif // HYPERION_RANDOM_SAMPLER

  for (std::size_t i = 0; i < sets; i++) {
#ifdef HYPERION_RANDOM_SAMPLER
    // cache_set_idx = i * window_size + dis(gen);
    cache_set_idx = i * window_size;

    this->_setLUT[cache_set_idx] = i;
#else
    this->_setLUT[i] = i;
#endif //HYPERION_RANDOM_SAMPLER
  }
}

Sampler::~Sampler () {

}

/**
 * @brief
 * @param setIdx
 */
bool Sampler::isSampledSet (std::size_t setIdx) const {
  return (this->_setLUT.find(setIdx) != this->_setLUT.end());
}

/**
 * @brief
 * @param idx
 */
std::size_t Sampler::translateSetIndex (std::size_t idx) {
  if (!this->isSampledSet(idx)) {
    // TODO: Error handling. Throw some excpetion.
  }

  return this->_setLUT[idx];
}

std::size_t Sampler::size () const {
  return this->_sets.size();
}

std::size_t Sampler::associativity () const {
  return this->_associativity;
}

void Sampler::access (Predictor::HyperionPolicy *policy) {
  std::size_t sampledSet,
              sampledTag,
              victimIdx;
  uint16_t current_quanta,
           last_quanta,
           current_timer;
  bool wrap;
  SamplerSet::SamplerBlockVectorIterator it;
  Hyperion::Features::BaseFeature::TraceType trace;

  // Computing the required information.
  sampledSet = this->_setLUT[policy->currentSet()];
  sampledTag = Helpers::extractPartialTag (policy->currentAddress ());

  current_quanta = (*this)[sampledSet].timer() % OPTGEN_VECTOR_SIZE;

  it = std::find((*this)[sampledSet].begin(), (*this)[sampledSet].end(), sampledTag);

  trace = policy->perceptron ()->buildTrace (policy);

  // This line has been used before.
  if (it != ((*this)[sampledSet]).end() && policy->currentType () != PREFETCH) {
    current_timer = (*this) [sampledSet].timer ();

    if (current_timer < it->lastQuanta ()) {
      current_timer += OPTGEN_TIMER_SIZE;
    }

    wrap = ((current_timer - it->lastQuanta ()) > OPTGEN_VECTOR_SIZE);
    last_quanta = it->lastQuanta () % OPTGEN_VECTOR_SIZE;

    /*
     * OPTgen algortihm would have cache this block. Kind of training negatively
     * the predictor for this block.
     */
    if (!wrap && ((*this) [sampledSet]).optgen ().should_cache (current_quanta, last_quanta)) {
      if (it->prefetched ()) {
        policy->perceptronPrefetch ()->train (*it, it->lastTrace (), Predictor::Perceptron::OPTgenHit);
      } else  {
        policy->perceptron ()->train (*it, it->lastTrace (), Predictor::Perceptron::OPTgenHit);
      }
    }
    /*
     * Conversely, the OPTgen algorithm just gave us an insight that this block
     * should not be cached. As a consequence, we should train the predictor
     * positively for this block.
     */
    else {
      if (it->prefetched ()) {
        policy->perceptronPrefetch ()->train (*it, it->lastTrace (), Predictor::Perceptron::OPTgenMiss);
      } else {
        policy->perceptron ()->train (*it, it->lastTrace (), Predictor::Perceptron::OPTgenMiss);
      }
    }

    ((*this) [sampledSet]).optgen ().add_access (current_quanta);

    // TODO: Update the LRU position in the sampler.
    for (SamplerSet::SamplerBlockVectorIterator _it = ((*this)[sampledSet]).begin ();
      _it != ((*this)[sampledSet]).end ();
      ++_it) {
      if (_it->lru () < it->lru ()) {
        _it->demote ();

        policy->perceptron ()->train (*_it, _it->lastTrace (), Predictor::Perceptron::SamplerDemotion);
      }
    }

    it->promote ();

    it->setPrefeteched (false);
  }
  // This is the first time we are seeing this line.
  else if (it == ((*this)[sampledSet]).end()) {
    SamplerBlock newBlock, evictedBlock;
    newBlock.setPartialTag (sampledTag);
    newBlock.setLastTrace (trace);

    victimIdx = ((*this) [sampledSet]).findVictim ();
    evictedBlock = (*this) (sampledSet, victimIdx);

    // We have to train the predictor for the evicted block. It is treated as a miss.
    policy->perceptron ()->train (evictedBlock, evictedBlock.lastTrace (), Predictor::Perceptron::OPTgenMiss);

    // Replacing the sampler victim block by a new one.
    (*this) (sampledSet, victimIdx) = newBlock;

    if (policy->currentType () == PREFETCH) {
      newBlock.setPrefeteched (true);
      ((*this)[sampledSet]).optgen ().add_prefetch (current_quanta);
    } else {
      ((*this)[sampledSet]).optgen ().add_access (current_quanta);
    }

    it = std::find((*this)[sampledSet].begin(), (*this)[sampledSet].end(), sampledTag);

    for (SamplerSet::SamplerBlockVectorIterator _it = ((*this)[sampledSet]).begin ();
      _it != ((*this)[sampledSet]).end ();
      ++_it) {
      if (_it != it) {
        _it->demote ();

        policy->perceptron ()->train (*_it, _it->lastTrace (), Predictor::Perceptron::SamplerDemotion);
      }
    }
  }
  else if (it != ((*this)[sampledSet]).end() && policy->currentType () == PREFETCH) {
    last_quanta = it->lastQuanta () % OPTGEN_VECTOR_SIZE;

    if ((*this) [sampledSet].timer () - it->lastQuanta () < 5 * NUM_CPUS) {
     if ((*this) [sampledSet].optgen ().should_cache (current_quanta, last_quanta)) {
       if (it->prefetched ()) {
         policy->perceptronPrefetch ()->train (*it, it->lastTrace (), Predictor::Perceptron::OPTgenHit);
       } else {
         policy->perceptron ()->train (*it, it->lastTrace (), Predictor::Perceptron::OPTgenHit);
       }
     }
    }

    it->setPrefeteched (true);

    ((*this) [sampledSet]).optgen ().add_prefetch (current_quanta);

    for (SamplerSet::SamplerBlockVectorIterator _it = ((*this)[sampledSet]).begin ();
      _it != ((*this)[sampledSet]).end ();
      ++_it) {
      if (_it->lru () < it->lru ()) {
        _it->demote ();

        policy->perceptron ()->train (*_it, _it->lastTrace (), Predictor::Perceptron::SamplerDemotion);
      }
    }

    it->promote ();
  }

  it->setPC (policy->currentPC ());
  it->setLastTrace (trace);
  it->setLastConfValue ((*policy->perceptron ()) (trace));

  if (policy->currentType () == PREFETCH) {
    it->setLastConfValue ((*policy->perceptronPrefetch ()) (trace));
  }

  it->setLastQuanta ((*this)[sampledSet].timer ());

  (*this)[sampledSet].setTimer (((*this)[sampledSet].timer() + 1) % OPTGEN_TIMER_SIZE);
}

const SamplerSet& Sampler::operator [] (std::size_t idx) const {
  if (idx >= this->size()) {
    // TODO: Error handling.
  }

  return this->_sets[idx];
}

SamplerSet& Sampler::operator [] (std::size_t idx) {
  if (idx >= this->size()) {
    // TODO: Error handling.
  }

  return this->_sets[idx];
}

const SamplerBlock& Sampler::operator () (std::size_t idx1, std::size_t idx2) const {
  return this->operator [] (idx1)[idx2];
}

SamplerBlock& Sampler::operator () (std::size_t idx1, std::size_t idx2) {
  return this->operator [] (idx1)[idx2];
}
