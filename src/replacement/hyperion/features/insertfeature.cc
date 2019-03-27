#include "insertfeature.hh"

using namespace Hyperion::Features;

InsertFeature::InsertFeature (uint8_t associativity, bool xored) :
  BaseFeature(associativity, xored) {

}

InsertFeature::~InsertFeature () {

}

InsertFeature::OutputType InsertFeature::build (const FeatureParameterList &params) {
  return this->xored () ? (params[0] ^ params[1]) : params[0];
}

InsertFeature::FeatureType InsertFeature::type () const {
  return InsertFeature::Insert;
}
