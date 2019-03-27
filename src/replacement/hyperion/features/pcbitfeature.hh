#ifndef __HYPERION_FEATURES_PCBITFEATURE_HH__
#define __HYPERION_FEATURES_PCBITFEATURE_HH__

#include <cstdint>
#
#include "basefeature.hh"

namespace Hyperion {
  namespace Features {
    class PCBitFeature : public BaseFeature {
    public:
      PCBitFeature(uint8_t associativity, uint8_t bitIdx, bool xored);
      virtual ~PCBitFeature ();

      virtual OutputType build (const FeatureParameterList &params);
      virtual FeatureType type () const;

      uint8_t bitIdx () const;

    private:
      uint8_t _bitIdx;
    };
  }
}

#endif // __HYPERION_FEATURES_PCFEATURE_HH__
