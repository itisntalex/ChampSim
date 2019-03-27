#ifndef __HYPERION_FEATURES_BASEFEATURE_HH__
#define __HYPERION_FEATURES_BASEFEATURE_HH__

#include <cstdint>
#
#include <vector>
#include <map>

namespace Hyperion {
  namespace Features {
    /**
     * @brief
     */
    class BaseFeature {
    public:
      using FeatureParameter = uint64_t;
      using FeatureParameterList = std::vector<FeatureParameter>;

      using OutputType = uint64_t;

      using TraceType = std::map<Features::BaseFeature *, OutputType>;

      enum FeatureType {
        ProgramCounter,
        ProgramCounterBit,
        Address,
        Bias,
        Insert,
        Offset,
      };

      BaseFeature (uint8_t associativity, bool xored);
      virtual ~BaseFeature ();

      uint8_t associativity () const;
      bool xored () const;

      virtual OutputType build (const FeatureParameterList &params) = 0;
      virtual FeatureType type () const = 0;

    private:
      uint8_t _associativity;
      bool _xored;
    };
  }
}

#endif // __HYPERION_FEATURES_BASEFEATURE_HH__
