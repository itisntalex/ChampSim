#include "helpers.hh"
#
#include "config.hh"

using namespace Hyperion;

uint64_t Helpers::extractTag (uint64_t paddr) {
  return (paddr >> (static_cast<uint64_t>(std::log2(LLC_SET)) + 0x6));
}

uint64_t Helpers::extractPartialTag (uint64_t paddr) {
  return Helpers::extractTag (paddr) & HYPERION_SAMPLER_TAG_MASK;
}

uint64_t Helpers::CRC( uint64_t _blockAddress )
{
    static const unsigned long long crcPolynomial = 3988292384ULL;
    unsigned long long _returnVal = _blockAddress;
    for( unsigned int i = 0; i < 32; i++ )
        _returnVal = ( ( _returnVal & 1 ) == 1 ) ? ( ( _returnVal >> 1 ) ^ crcPolynomial ) : ( _returnVal >> 1 );
    return _returnVal;
}

uint8_t Helpers::probabilisticRRPV (int32_t v) {
  uint8_t r = 0;

  r =  (v + 256) / static_cast<int32_t>(std::exp2 (HYPERION_PERCEPTRON_OUTPUT_BITS - HYPERION_RRPV_BITS));

  r = static_cast<int32_t>(std::floor (r));

  return r;
}
