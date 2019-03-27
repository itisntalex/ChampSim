#ifndef __HYPERION_PREDICTOR_PERCEPTRON_HH__
#define __HYPERION_PREDICTOR_PERCEPTRON_HH__

#include <cstdint>
#
#include <list>
#include <vector>
#
#include "../features/basefeature.hh"

namespace Hyperion {
  namespace Sampler {
    class SamplerBlock;
  }

  namespace Predictor {
    class HyperionPolicy;

    /**
     * @brief
     */
    class Perceptron {
    public:
      using FeaturePointerVector = std::vector<Features::BaseFeature *>;

      using WeightType = int8_t;
      using OutputType = int32_t;

      using WeightTable = std::vector<WeightType>;
      using WeightTables = std::vector<WeightTable>;

      enum TrainingType {
        OPTgenHit = 0x1,
        OPTgenMiss = 0x2,
        SamplerDemotion = 0x4,
        LRUEviction = 0x8,

        TrainNeighbours = 0x10
      };

      Perceptron (const FeaturePointerVector &features);
      ~Perceptron ();

      const FeaturePointerVector& features () const;
      const WeightTables& weightTables () const;

      void train (const Sampler::SamplerBlock &block, const Features::BaseFeature::TraceType &trace, uint32_t type);

      Features::BaseFeature::TraceType buildTrace (const HyperionPolicy *policy);

      OutputType operator () (const HyperionPolicy *policy);
      OutputType operator () (const Features::BaseFeature::TraceType &trace);

    private:
      FeaturePointerVector _features;
      WeightTables _weightTables;
      WeightType **_weights;
      std::size_t *_weightsSize;
    };
  }
}

#endif // __HYPERION_PREDICTOR_PERCEPTRON_HH__
