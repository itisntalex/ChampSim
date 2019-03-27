#include <iostream>
#include <iomanip>
#
#include "pcbitfeature.hh"

using namespace Hyperion::Features;

/**
 *
 */
PCBitFeature::PCBitFeature (uint8_t associativity, uint8_t bitIdx, bool xored) :
  BaseFeature(associativity, xored), _bitIdx (bitIdx) {

}

/**
 *
 */
PCBitFeature::~PCBitFeature () {

}

/**
 * @brief
 * @param params A list of parameters for the feature to builda value that will
 * be used to index the associated table of weights.
 */
PCBitFeature::OutputType PCBitFeature::build (const FeatureParameterList &params) {
  return (params[0] & (1 << this->_bitIdx)) ? 0x1 : 0x0;
}

PCBitFeature::FeatureType PCBitFeature::type () const {
  return PCBitFeature::ProgramCounterBit;
}

/**
 * @brief
 */
uint8_t PCBitFeature::bitIdx () const {
  return this->_bitIdx;
}
