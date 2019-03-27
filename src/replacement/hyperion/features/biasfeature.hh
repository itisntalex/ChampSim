#ifndef __HYPERION_FEATURES_BIASFEATURE_HH__
#define __HYPERION_FEATURES_BIASFEATURE_HH__

#include <cstdint>
#
#include "basefeature.hh"

namespace Hyperion {
  namespace Features {
    class BiasFeature : public BaseFeature {
    public:
      BiasFeature (uint8_t associativity, bool xored);
      virtual ~BiasFeature ();

      virtual OutputType build (const FeatureParameterList &params);
      virtual FeatureType type () const;
    };
  }
}

#endif // __HYPERION_FEATURES_BIASFEATURE_HH__
