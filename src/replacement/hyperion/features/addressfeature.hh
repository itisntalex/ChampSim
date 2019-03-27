#ifndef __HYPERION_FEATURES_ADDRESSFEATURE_HH__
#define __HYPERION_FEATURES_ADDRESSFEATURE_HH__

#include <cstdint>
#
#include "basefeature.hh"

namespace Hyperion {
  namespace Features {
    class AddressFeature : public BaseFeature {
    public:
      AddressFeature(uint8_t associativity, uint8_t begin, uint8_t end, bool xored);
      virtual ~AddressFeature ();

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

#endif // __HYPERION_FEATURES_ADDRESSFEATURE_HH__
