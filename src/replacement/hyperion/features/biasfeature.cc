#include "biasfeature.hh"

using namespace Hyperion::Features;

BiasFeature::BiasFeature (uint8_t associativity, bool xored) :
  BaseFeature (associativity, xored) {

}

BiasFeature::~BiasFeature () {

}

BiasFeature::OutputType BiasFeature::build (const FeatureParameterList &params) {
  return this->xored() ? params[0] : 0x0;
}

BiasFeature::FeatureType BiasFeature::type () const {
  return BiasFeature::Bias;
}
