#include <cmath>
#
#include "../helpers.hh"
#include "perceptron.hh"
#include "hyperionpolicy.hh"
#
#include "../features/pcfeature.hh"

using namespace Hyperion;
using namespace Hyperion::Predictor;

Perceptron::Perceptron (const FeaturePointerVector &features) :
  _features(features), _weightTables() {
  std::size_t table_entries = 0;
  FeaturePointerVector::difference_type dist = 0x0;
  Features::BaseFeature* current_feature = nullptr;

  // Resizing the wight tables container.
  // this->_weightTables.resize(this->_features.size());
  //
  // for (FeaturePointerVector::iterator it = this->_features.begin();
  //      it != this->_features.end();
  //      ++it) {
  //   current_feature = *it;
  //   dist = (it - this->_features.begin());
  //
  //   switch (current_feature->type()) {
  //     case Features::BaseFeature::Insert:
  //       this->_weightTables[dist].resize(0x2, 0x0);
  //       break;
  //     case Features::BaseFeature::Bias:
  //       if (current_feature->xored()) {
  //         this->_weightTables[dist].resize(0x100, 0x0);
  //       } else {
  //         this->_weightTables[dist].resize(0x1, 0x0);
  //       }
  //       break;
  //     case Features::BaseFeature::Offset:
  //       if (current_feature->xored ()) {
  //         this->_weightTables[dist].resize (0x100, 0x0);
  //       } else {
  //         this->_weightTables[dist].resize (0x80, 0x0);
  //       }
  //     default:
  //       this->_weightTables[dist].resize(0x100, 0x0);
  //       break;
  //   }
  // }

  // TODO: Implement a new algorithm based on array instead of vectors.
  this->_weights = new WeightType* [this->_features.size ()];
  this->_weightsSize = new std::size_t [this->_features.size ()];

  for (std::size_t i = 0; i < this->_features.size (); i++) {
    current_feature = this->_features[i];
    switch (current_feature->type()) {
      case Features::BaseFeature::Insert:
      case Features::BaseFeature::ProgramCounterBit:
        table_entries = 2;
        break;
      case Features::BaseFeature::Bias:
        if (current_feature->xored()) {
          table_entries = 256;
          // table_entries = 0x2000;
        } else {
          table_entries = 1;
        }
        break;
      case Features::BaseFeature::Offset:
        if (current_feature->xored ()) {
          table_entries = 256;
          // table_entries = 0x2000;
        } else {
          table_entries = 64;
        }
      default:
        table_entries = 256;
        // table_entries = 0x2000;
        break;
    }

    this->_weights[i] = new WeightType[table_entries];
    this->_weightsSize[i] = table_entries;

    for (std::size_t j = 0; j < table_entries; j++) {
      this->_weights[i][j] = 0x0;
    }
  }
}

Perceptron::~Perceptron () {

}

const Perceptron::FeaturePointerVector& Perceptron::features () const {
  return this->_features;
}

const Perceptron::WeightTables& Perceptron::weightTables () const {
  return this->_weightTables;
}

void Perceptron::train (const Sampler::SamplerBlock &block, const Features::BaseFeature::TraceType &trace, uint32_t type) {
  bool prediction, correct, do_train = false;
  std::size_t dist = 0x0;
  WeightType limit = 0x0,
             delta = 0x0;

  prediction = (block.lastConfValue () >= 200);
  correct = (prediction && (type & OPTgenMiss)) ||
            (!prediction && (type & OPTgenHit)) ||
            !(type & SamplerDemotion) ||
            !(type & LRUEviction);

  /*
   * If the magnitude of the last confidence value doesn't exceed a threshold,
   * we train the perceptron in order to get upcoming confidence value colser to
   * this threshold and make predictions more accurate.
   */
  if (std::abs (block.lastConfValue ()) < 100) {
    do_train = true;
  }

  /*
   * If the prediction is wrong, we should train the predictor.
   */
  if (!correct) {
    do_train = true;
  }

  if (!do_train) {
    return;
  }

  for (Features::BaseFeature::TraceType::const_iterator it = trace.begin();
       it != trace.end();
       ++it) {
    // Checking som preconditions prior to triggering the learning process.
    if ((type & OPTgenHit)  && block.lru () >= it->first->associativity ()) {
      continue;
    }

    if ((type & OPTgenMiss) && block.lru () != it->first->associativity ()) {
      continue;
    }

    if ((type & SamplerDemotion) && block.lru () < it->first->associativity ()) {
      continue;
    }

    if (type & LRUEviction) {
      // When we trigger a training for an LRU eviction, we learn anyways.
    }

    if (type & OPTgenHit) {
      if (it->first->type () == Features::BaseFeature::ProgramCounterBit) {
        limit = -2;
      } else {
        // limit = HYPERION_PERCEPTRON_WEIGHTS_MIN;
        limit = INT8_MIN;
      }

      delta = -1;

      if (type & TrainNeighbours) {
        delta = -2;
      }

      this->_weights[dist][it->second] = std::max (limit, static_cast<WeightType> (this->_weights[dist][it->second] + delta));
    } else if ((type & OPTgenMiss) || (type & SamplerDemotion) || (type & LRUEviction)) {
      if (it->first->type () == Features::BaseFeature::ProgramCounterBit) {
        limit = 1;
      } else {
        // limit = HYPERION_PERCEPTRON_WEIGHTS_MAX;
        limit = INT8_MAX;
      }

      delta = 1;

      if (type & TrainNeighbours) {
        delta = 2;
      }

      this->_weights[dist][it->second] = std::min (limit, static_cast<WeightType> (this->_weights[dist][it->second] + delta));
    }

    dist++;
  }

  dist = 0x0;

  if (type & TrainNeighbours) {
    // TODO: Try somthing here
  }
}

Features::BaseFeature::TraceType Perceptron::buildTrace (const HyperionPolicy *policy) {
  Features::BaseFeature::TraceType trace;
  Features::BaseFeature* current_feature = nullptr;
  std::ptrdiff_t dist = 0x0;
  std::size_t hash = 0x0;

  for (FeaturePointerVector::iterator it = this->_features.begin();
       it != this->_features.end();
       ++it) {
    uint8_t which;
    Features::BaseFeature::FeatureParameterList params;

    current_feature = *it;
    dist = it - this->_features.begin();

    switch (current_feature->type()) {
      case Features::BaseFeature::ProgramCounterBit:
        params.push_back (policy->currentPC ());

        break;

      case Features::BaseFeature::ProgramCounter:
       which = dynamic_cast<Features::PCFeature *>(current_feature)->which();

       if (which == 0) {
         params.push_back(policy->currentPC());
       } else {
         params.push_back(
           policy->pcHistoryElement(
             policy->currentCoreIdx(),
             dynamic_cast<Features::PCFeature *>(current_feature)->which() - 1
           )
         );
       }

       params.push_back(policy->currentPC());

       break;

     case Features::BaseFeature::Address:
       params.push_back(policy->currentAddress());
       params.push_back(policy->currentPC());

       break;

     case Features::BaseFeature::Bias:
      params.push_back(policy->currentPC ());

      break;

    case Features::BaseFeature::Insert:
      params.push_back(static_cast<Features::BaseFeature::FeatureParameter> (!policy->hit ()));
      params.push_back(policy->currentPC ());

      break;

    case Features::BaseFeature::Offset:
      params.push_back(policy->currentAddress () & 0x7F);
      params.push_back(policy->currentPC ());

      break;

     default:
       break;
    }

    // TODO: The hashing function should be changed to something smarter.
    hash = current_feature->build(params) % this->_weightsSize[dist];
    // hash = Helpers::CRC (current_feature->build(params)) % this->_weightsSize[dist];

    trace[current_feature] = static_cast<Features::BaseFeature::TraceType::mapped_type>(hash);
  }

  return trace;
}

Perceptron::OutputType Perceptron::operator () (const HyperionPolicy *policy) {
  return this->operator () (this->buildTrace (policy));
}

Perceptron::OutputType Perceptron::operator () (const Features::BaseFeature::TraceType &trace) {
  OutputType r = 0;
  std::ptrdiff_t dist = 0x0;

  for (Features::BaseFeature::TraceType::const_iterator it = trace.begin();
       it != trace.end();
       ++it) {
    // r += this->_weightTables[dist][it->second];
    r += this->_weights[dist][it->second];

    dist++;
  }

  r = std::max(HYPERION_PERCEPTRON_OUTPUT_MIN, std::min(HYPERION_PERCEPTRON_OUTPUT_MAX, r));

  return r;
}
