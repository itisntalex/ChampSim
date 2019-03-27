#include <algorithm>
#
#include "shadowcache.hh"

using namespace Hyperion::ShadowCache;

ShadowCache::ShadowCache (std::size_t sets, std::size_t ways) :
  _cache (sets, std::vector<CacheBlock> (ways, CacheBlock ())),
  _bypassRegister (sets) {

}

void ShadowCache::markBypassed (std::size_t setIdx, const CacheBlock &b) {
  std::list <CacheBlock>::iterator it, begin, end;

  if (setIdx >= this->_bypassRegister.size ()) {
    // TODO: Handle error.
  }

  begin = this->_bypassRegister[setIdx].begin ();
  end = this->_bypassRegister[setIdx].end ();

  it = std::find (begin, end, b);

  if (it == end) {
    this->_bypassRegister[setIdx].push_back (b);
  }
}

void ShadowCache::unmarkBypassed (std::size_t setIdx, const CacheBlock &b) {
  std::list <CacheBlock>::iterator it, begin, end;

  if (setIdx >= this->_bypassRegister.size ()) {
    // TODO: Handle error.
  }

  begin = this->_bypassRegister[setIdx].begin ();
  end = this->_bypassRegister[setIdx].end ();

  it = std::find (begin, end, b);

  if (it != end) {
    this->_bypassRegister[setIdx].erase (it);
  }
}

bool ShadowCache::markedBypassed (std::size_t setIdx, const CacheBlock &b) const {
  std::list <CacheBlock>::const_iterator begin, end;

  begin = this->_bypassRegister[setIdx].cbegin ();
  end = this->_bypassRegister[setIdx].cend ();

  return (std::find (begin, end, b) != end);
}

bool ShadowCache::isCached (std::size_t setIdx, const CacheBlock &b) const {
  std::vector<CacheBlock>::const_iterator begin, end;

  begin = this->_cache[setIdx].cbegin ();
  end = this->_cache[setIdx].cend ();

  return (std::find (begin, end, b) != end);
}

CacheBlock& ShadowCache::findVictim (std::size_t setIdx) {
  uint8_t max_rrip = 0x0;
  std::vector<CacheBlock>::iterator itLruVictim;

  for (std::vector<CacheBlock>::iterator it = this->_cache[setIdx].begin ();
       it != this->_cache[setIdx].end ();
       ++it) {
    if (it->rrpv () == HYPERION_RRPV_MAX) {
      return *it;
    }
  }

  for (std::vector<CacheBlock>::iterator it = this->_cache[setIdx].begin ();
       it != this->_cache[setIdx].end ();
       ++it) {
    if (it->rrpv () >= max_rrip) {
      max_rrip = it->rrpv ();
      itLruVictim = it;
    }
  }

  return *itLruVictim;
}

CacheBlock& ShadowCache::operator () (std::size_t setIdx, std::size_t wayIdx) {
  return _cache [setIdx][wayIdx];
}
