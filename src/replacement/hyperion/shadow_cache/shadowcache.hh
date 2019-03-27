#ifndef __HYPERION_SHADOW_CACHE_SHADOWCACHE_HH__
#define __HYPERION_SHADOW_CACHE_SHADOWCACHE_HH__

#include <vector>
#include <list>
#
#include "cacheblock.hh"
#
#include "../config.hh"

namespace Hyperion {
  namespace ShadowCache {
    class ShadowCache {
    public:
      ShadowCache (std::size_t sets, std::size_t ways);

      void markBypassed (std::size_t setIdx, const CacheBlock &b);
      void unmarkBypassed (std::size_t setIdx, const CacheBlock &b);

      bool markedBypassed (std::size_t setIdx, const CacheBlock &b) const;
      bool isCached (std::size_t setIdx, const CacheBlock &b) const;

      CacheBlock& findVictim (std::size_t setIdx);

      CacheBlock& operator () (std::size_t setIdx, std::size_t wayIdx);

    private:
      std::vector <std::vector <CacheBlock> > _cache;
      std::vector <std::list <CacheBlock> > _bypassRegister;
    };
  }
}

#endif // __HYPERION_SHADOW_CACHE_SHADOWCACHE_HH__
