#include "offsetfeature.hh"

using namespace Hyperion::Features;

/**
 *
 */
OffsetFeature::OffsetFeature (uint8_t associativity, uint8_t begin, uint8_t end, bool xored) :
  BaseFeature(associativity, xored), _begin(begin), _end(end) {

}

/**
 *
 */
OffsetFeature::~OffsetFeature () {

}

/**
 * @brief
 * @param params A list of parameters for the feature to builda value that will
 * be used to index the associated table of weights.
 */
OffsetFeature::OutputType OffsetFeature::build (const FeatureParameterList &params) {
  uint64_t r = 0x0,
           mask = 0x0;

  // First, we calculate a mask to isolate some bits of the w-th most recent PC.
  for (std::size_t i = this->_begin; i <= this->_end; i++) {
    mask |= (1 << i);
  }

  // We extract some bits from the w-th most recent PC using the mask calculated above.
  r = (params.at(0) & mask);

  // If we asked for it, we xor the result with the current PC.
  if (this->xored()) {
    r ^= params.at(1);
  }

  return r;
}

OffsetFeature::FeatureType OffsetFeature::type () const {
  return OffsetFeature::Offset;
}

/**
 * @brief
 */
uint8_t OffsetFeature::begin () const {
  return this->_begin;
}

/**
 * @brief
 */
uint8_t OffsetFeature::end () const {
  return this->_end;
}
