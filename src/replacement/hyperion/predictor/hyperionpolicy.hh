#ifndef __HYPERION_PREDICTOR_PREDICTOR_HH__
#define __HYPERION_PREDICTOR_PREDICTOR_HH__

#include <cstdint>
#
#include <list>
#
#include "../config.hh"
#include "perceptron.hh"
#include "../sampler/sampler.hh"
#include "../shadow_cache/shadowcache.hh"

// Forward declarations.
class BLOCK;

namespace Hyperion {
  namespace Predictor {
    class HyperionPolicy {
    public:
      using ProgramCounterType = uint64_t;
      using AddressType = uint64_t;

      using PCHistory = ProgramCounterType**;

      HyperionPolicy (const Perceptron::FeaturePointerVector &features);
      ~HyperionPolicy ();

      void updatePolicyState (uint32_t cpu, uint32_t set, uint32_t way, uint64_t full_addr, uint64_t ip, uint64_t victim_addr, uint32_t type, uint8_t hit);
      uint8_t findVictim (uint32_t cpu, uint64_t instr_id, uint32_t set, const ::BLOCK *current_set, uint64_t ip, uint64_t full_addr, uint32_t type);

      void finalStats ();

      Perceptron* perceptron ();
      Perceptron* perceptronPrefetch ();

      uint8_t currentCoreIdx () const;
      ProgramCounterType currentPC () const;
      AddressType currentAddress () const;
      std::size_t currentSet () const;
      uint32_t currentType () const;
      bool hit () const;
      ProgramCounterType pcHistoryElement (uint32_t cpu, uint8_t idx) const;

      static Perceptron::OutputType threshold ();
      static void setThreshold (Perceptron::OutputType v);

    private:
      Perceptron _perc;
      Perceptron _percPrefetch;
      Sampler::Sampler _samp;

      uint8_t _currentCoreIdx;
      ProgramCounterType _currentPC;
      AddressType _currentAddress;
      std::size_t _currentSet;
      uint32_t _currentType;
      bool _hit;

      PCHistory _pchistory;

      uint8_t** _rrpv;

      ShadowCache::ShadowCache _shadowCache;

      static Perceptron::OutputType _threshold;

      std::map<std::string, uint32_t> _metrics;
    };
  }
}

#endif // __HYPERION_PREDICTOR_PREDICTOR_HH__
