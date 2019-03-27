#ifndef __HYPERION_SAMPLER_SAMPLERSET_HH__
#define __HYPERION_SAMPLER_SAMPLERSET_HH__

#include <cstdint>
#
#include <vector>
#
#include "samplerblock.hh"
#include "../optgen.h"

namespace Hyperion {
  namespace Sampler {
    class SamplerSet {
    public:
      // TODO: Create type aliasing for the sampler block type and derivatives.
      using SamplerBlockVector = std::vector<SamplerBlock>;
      using SamplerBlockVectorIterator = SamplerBlockVector::iterator;

      SamplerSet (uint8_t associativity = 0xF);
      ~SamplerSet ();

      std::size_t size () const;

      uint16_t timer ();
      OPTgen& optgen ();

      void setTimer (uint16_t v);

      std::size_t findVictim () const;

      SamplerBlockVectorIterator begin ();
      SamplerBlockVectorIterator end ();

      SamplerBlock& operator [] (std::size_t idx);
      const SamplerBlock& operator [] (std::size_t idx) const;

    private:
      uint8_t _associativity;
      SamplerBlockVector _blocks;

      // OPTgen/Hawkeye-related stuff. Considered as a block box.
      OPTgen _optgen;
      uint16_t _timer;
    };
  }
}

#endif // __HYPERION_SAMPLER_SAMPLERSET_HH__
