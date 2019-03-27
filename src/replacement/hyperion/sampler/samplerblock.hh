#ifndef __HYPERION_SAMPLER_SAMPLERBLOCK_HH__
#define __HYPERION_SAMPLER_SAMPLERBLOCK_HH__

#include <cstdint>
#
#include "../features/basefeature.hh"
#include "../predictor/perceptron.hh"

namespace Hyperion {
  namespace Sampler {
    class SamplerBlock {
    public:
      SamplerBlock ();
      SamplerBlock (const SamplerBlock &other);
      ~SamplerBlock ();

      uint64_t PC () const;
      uint64_t partialTag () const;
      const Features::BaseFeature::TraceType& lastTrace () const;
      Predictor::Perceptron::OutputType lastConfValue () const;
      uint8_t lru () const;
      uint16_t lastQuanta () const;
      bool prefetched () const;

      void setPC (uint64_t v);
      void setPartialTag (uint64_t v);
      void setLastTrace (const Features::BaseFeature::TraceType& v);
      void setLastConfValue (const Predictor::Perceptron::OutputType& v);
      void setLastQuanta (uint16_t v);
      void setLRU (uint8_t v);
      void setPrefeteched (bool v);

      void promote ();
      void demote ();

      SamplerBlock& operator = (const SamplerBlock &other);

      bool operator == (uint64_t tag);

    private:
      uint64_t _PC;
      uint64_t _partialTag;
      uint8_t _lru;
      uint16_t _lastQuanta;
      bool _prefetched;
      Predictor::Perceptron::OutputType _lastConfValue;
      Features::BaseFeature::TraceType _lastTrace;
    };
  }
}

#endif // __HYPERION_SAMPLER_SAMPLERBLOCK_HH__
