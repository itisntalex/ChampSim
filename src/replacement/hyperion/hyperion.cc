#include <iomanip>
#
#include "cache.h"
#
#include "predictor/hyperionpolicy.hh"
#include "features/pcfeature.hh"
#include "features/addressfeature.hh"
#include "features/biasfeature.hh"
#include "features/insertfeature.hh"
#include "features/offsetfeature.hh"
#include "features/pcbitfeature.hh"

using namespace Hyperion;
using namespace Hyperion::Predictor;
using namespace Hyperion::Features;

static HyperionPolicy* policy;
static Perceptron::FeaturePointerVector features;

/**
 * @brief Initialize the last level cache replacement policy's states and
 * data structures.
 */
void CACHE::llc_initialize_replacement () {
  // features.push_back (new AddressFeature(11, 8, 19, false));
  // features.push_back (new BiasFeature (6, true));
  // features.push_back (new InsertFeature (15, false));
  // features.push_back (new InsertFeature (16, true));
  // features.push_back (new InsertFeature (6, true));
  // features.push_back (new OffsetFeature (15, 1, 6, true));
  // features.push_back (new OffsetFeature (15, 3, 7, false));
  // features.push_back (new PCFeature (11, 2, 24, 4, true));
  // features.push_back (new PCFeature (15, 14, 32, 6, false));
  // features.push_back (new PCFeature (15, 5, 28, 0, true));
  // features.push_back (new PCFeature (16, 0, 16, 8, true));
  // features.push_back (new PCFeature (17, 6, 20, 0, true));
  // features.push_back (new PCFeature (6, 12, 14, 10, true));
  // features.push_back (new PCFeature (7, 1, 24, 11, false));
  // features.push_back (new PCFeature (7, 14, 43, 11, false));
  // features.push_back (new PCFeature (8, 1, 61, 11, false));

  features.push_back (new BiasFeature (0x12, true));
  // features.push_back (new AddressFeature (0x12, 6, 16, false));

  for (size_t i = 0; i < 64; i++) {
    features.push_back (new PCBitFeature (0x12, i, false));
  }

  policy = new HyperionPolicy (features);
}

/**
 * @brief
 * @param cpu
 * @param instr_id
 * @param set
 * @param current_set
 * @param ip
 * @param full_addr
 * @param type
 */
uint32_t CACHE::llc_find_victim (uint32_t cpu, uint64_t instr_id, uint32_t set, const BLOCK *current_set, uint64_t ip, uint64_t full_addr, uint32_t type) {
    uint8_t hyperion_victim = policy->findVictim (cpu, instr_id, set, current_set, ip, full_addr, type);

    // if (hyperion_victim == 0xFF) {
    //   return lru_victim(cpu, instr_id, set, current_set, ip, full_addr, type);
    // }

    return hyperion_victim;
}

/**
 * @brief Called on every cache hit and cache fill.
 * @param cpu
 * @param set
 * @param way
 * @param full_addr
 * @param ip
 * @param victim_addr
 * @param type
 * @param hit
 */
void CACHE::llc_update_replacement_state (uint32_t cpu, uint32_t set, uint32_t way, uint64_t full_addr, uint64_t ip, uint64_t victim_addr, uint32_t type, uint8_t hit) {
  policy->updatePolicyState (cpu, set, way, full_addr, ip, victim_addr, type, hit);
}

/**
 * @brief This function will print some final stats at the end of the simulation.
 * This is one is optional.
 */
void CACHE::llc_replacement_final_stats () {
  policy->finalStats ();
  
  delete policy;

  for (Perceptron::FeaturePointerVector::iterator it = features.begin ();
       it != features.end ();
       ++it) {
    if (*it != nullptr) {
      delete *it;
    }
  }
}
