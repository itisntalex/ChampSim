#include <iostream>
#include <iomanip>
#
#include "pcfeature.hh"

using namespace Hyperion::Features;

/**
 *
 */
PCFeature::PCFeature (uint8_t associativity, uint8_t begin, uint8_t end, uint8_t which, bool xored) :
  BaseFeature(associativity, xored), _begin(begin), _end(end), _which(which) {

}

/**
 *
 */
PCFeature::~PCFeature () {

}

/**
 * @brief
 * @param params A list of parameters for the feature to builda value that will
 * be used to index the associated table of weights.
 */
PCFeature::OutputType PCFeature::build (const FeatureParameterList &params) {
  uint64_t r = 0x0,
           mask = (1ULL << (this->_end - this->_begin)) - 1ULL;

  // for (std::size_t i = this->_begin; i < this->_end; i++) {
  //  mask |= (1ULL << i);
  // }

  // We extract some bits from the w-th most recent PC using the mask calculated above.
  r = ((params.at(0) >> this->_begin) & mask);
  // r = (params.at (0) & mask);

  // If we asked for it, we xor the result with the current PC.
  if (this->xored()) {
    r ^= params.at(1);
  }

  return r;
}

PCFeature::FeatureType PCFeature::type () const {
  return PCFeature::ProgramCounter;
}

/**
 * @brief
 */
uint8_t PCFeature::begin () const {
  return this->_begin;
}

/**
 * @brief
 */
uint8_t PCFeature::end () const {
  return this->_end;
}

/**
 * @brief
 */
uint8_t PCFeature::which () const {
  return this->_which;
}
