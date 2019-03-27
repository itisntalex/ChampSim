#include "cacheblock.hh"

using namespace Hyperion::ShadowCache;

CacheBlock::CacheBlock () : _rrpv (HYPERION_RRPV_MAX), _addr (0x0) {

}

CacheBlock::CacheBlock (uint8_t rrpv, uint64_t addr) :
  _rrpv (rrpv), _addr (addr) {

}

uint8_t& CacheBlock::rrpv () {
  return this->_rrpv;
}

uint64_t& CacheBlock::addr () {
  return this->_addr;
}

bool CacheBlock::operator == (const CacheBlock &other) const {
  return (this->_addr == other._addr);
}
