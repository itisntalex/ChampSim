#include "addressfeature.hh"

using namespace Hyperion::Features;

AddressFeature::AddressFeature (uint8_t associativity, uint8_t begin, uint8_t end, bool xored) :
  BaseFeature(associativity, xored), _begin(begin), _end(end) {

}

AddressFeature::~AddressFeature () {

}

AddressFeature::OutputType AddressFeature::build (const FeatureParameterList &params) {
  uint64_t r = 0x0,
           mask = (1ULL << (this->_end - this->_begin)) - 1ULL;

  // for (std::size_t i = this->_begin; i < this->_end; i++) {
  //   mask |= (1ULL << i);
  // }

  r = ((params.at(0) >> this->_begin) & mask);
  // r = (params.at (0) & mask);

  if (this->xored()) {
    r ^= params[1];
  }

  return r;
}

AddressFeature::FeatureType AddressFeature::type () const {
  return AddressFeature::Address;
}
