#ifndef __HYPERION_FEATURES_OFFSETFEATURE_HH__
#define __HYPERION_FEATURES_OFFSETFEATURE_HH__

#include <cstdint>
#
#include "basefeature.hh"

namespace Hyperion {
  namespace Features {
    class OffsetFeature : public BaseFeature {
    public:
      OffsetFeature(uint8_t associativity, uint8_t begin, uint8_t end, bool xored);
      virtual ~OffsetFeature ();

      virtual OutputType build (const FeatureParameterList &params);
      virtual FeatureType type () const;

      uint8_t begin () const;
      uint8_t end () const;

    private:
      uint8_t _begin;
      uint8_t _end;
    };
  }
}

#endif // __HYPERION_FEATURES_OFFSETFEATURE_HH__
