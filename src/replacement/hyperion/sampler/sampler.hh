#ifndef __HYPERION_SAMPLER_SAMPLER_HH__
#define __HYPERION_SAMPLER_SAMPLER_HH__

#include <cstdint>
#
#include <vector>
#
#include "samplerset.hh"

// Forward declarations.
namespace Hyperion {
  namespace Predictor {
    class HyperionPolicy;
  }
}

namespace Hyperion {
  namespace Sampler {
    class Sampler {
    public:
      using SamplerSetVector = std::vector<SamplerSet>;
      using SamplerLUT = std::map<std::size_t, std::size_t>;

      Sampler (std::size_t sets, std::size_t associativity);
      ~Sampler ();

      bool isSampledSet (std::size_t setIdx) const;
      std::size_t translateSetIndex (std::size_t idx);

      std::size_t size () const;

      std::size_t associativity () const;

      void access (Predictor::HyperionPolicy *policy);

      SamplerSet& operator [] (std::size_t idx);
      const SamplerSet& operator [] (std::size_t idx) const;

      SamplerBlock& operator () (std::size_t idx1, std::size_t idx2);
      const SamplerBlock& operator () (std::size_t idx1, std::size_t idx2) const;

    private:
      SamplerSetVector _sets;
      SamplerLUT _setLUT;
      std::size_t _associativity;
    };
  }
}

#endif // __HYPERION_SAMPLER_SAMPLER_HH__
