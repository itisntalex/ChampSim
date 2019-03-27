#ifndef __HYPERION_FEATURES_INSERTFEATURE_HH__
#define __HYPERION_FEATURES_INSERTFEATURE_HH__

#include <cstdint>
#
#include "basefeature.hh"

namespace Hyperion {
  namespace Features {
    class InsertFeature : public BaseFeature {
    public:
      InsertFeature (uint8_t associativity, bool xored);
      virtual ~InsertFeature ();

      virtual OutputType build (const FeatureParameterList &params);
      virtual FeatureType type () const;
    };
  }
}

#endif // __HYPERION_FEATURES_INSERTFEATURE_HH__
