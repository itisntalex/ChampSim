#ifndef __HYPERION_FEATURES_PCFEATURE_HH__
#define __HYPERION_FEATURES_PCFEATURE_HH__

#include <cstdint>
#
#include "basefeature.hh"

namespace Hyperion {
  namespace Features {
    class PCFeature : public BaseFeature {
    public:
      PCFeature(uint8_t associativity, uint8_t begin, uint8_t end, uint8_t which, bool xored);
      virtual ~PCFeature ();

      virtual OutputType build (const FeatureParameterList &params);
      virtual FeatureType type () const;

      uint8_t begin () const;
      uint8_t end () const;
      uint8_t which () const;

    private:
      uint8_t _begin;
      uint8_t _end;
      uint8_t _which;
    };
  }
}

#endif // __HYPERION_FEATURES_PCFEATURE_HH__
