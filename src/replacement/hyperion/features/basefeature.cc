#include "basefeature.hh"

using namespace Hyperion::Features;

BaseFeature::BaseFeature (uint8_t associativity, bool xored) :
  _associativity(associativity), _xored(xored) {

}

BaseFeature::~BaseFeature () {

}

uint8_t BaseFeature::associativity () const {
  return this->_associativity;
}

bool BaseFeature:: xored () const {
  return this->_xored;
}
