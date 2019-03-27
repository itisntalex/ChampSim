#ifndef __HYPERION_HELPERS_HH__
#define __HYPERION_HELPERS_HH__

#include <cstdint>
#include <cmath>
#
#include <cache.h>

namespace Hyperion {
  class Helpers {
  public:
    static uint64_t extractTag (uint64_t paddr);
    static uint64_t extractPartialTag (uint64_t paddr);
    static uint64_t CRC( uint64_t _blockAddress );
    static uint8_t probabilisticRRPV (int32_t v);
  };
}

#endif // __HYPERION_HELPERS_HH__
