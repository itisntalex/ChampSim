#ifndef __HYPERION_SHADOW_CACHE_CACHEBLOCK_HH__
#define __HYPERION_SHADOW_CACHE_CACHEBLOCK_HH__

#include <cstdint>
#
#include "../config.hh"

namespace Hyperion {
  namespace ShadowCache {
    class CacheBlock {
    public:
      CacheBlock ();
      CacheBlock (uint8_t rrpv, uint64_t addr);

      uint8_t& rrpv ();
      uint64_t& addr ();

      bool operator == (const CacheBlock &other) const;

    private:
      uint8_t _rrpv;
      uint64_t _addr;
    };
  }
}

#endif // __HYPERION_SHADOW_CACHE_CACHEBLOCK_HH__
