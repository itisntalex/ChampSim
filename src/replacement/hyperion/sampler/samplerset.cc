#include <cache.h>
#
#include "samplerset.hh"

using namespace Hyperion::Sampler;

SamplerSet::SamplerSet (uint8_t associativity) :
  _associativity(associativity), _blocks(associativity, SamplerBlock()), _timer(0x0) {
  for (std::size_t i = 0; i < associativity; i++) {
    this->_blocks[i].setLRU (i);
  }

  // Initializing the OPTgen stuff.
  this->_optgen.init(LLC_WAY - 2);
}

SamplerSet::~SamplerSet () {

}

std::size_t SamplerSet::size () const {
  return this->_blocks.size();
}

uint16_t SamplerSet::timer () {
  return this->_timer;
}

OPTgen& SamplerSet::optgen () {
  return this->_optgen;
}

void SamplerSet::setTimer (uint16_t v) {
  this->_timer = v;
}

std::size_t SamplerSet::findVictim () const {
  SamplerBlockVector::const_iterator begin = this->_blocks.cbegin();
  std::size_t idx = -1;

  for (SamplerBlockVector::const_iterator it = this->_blocks.cbegin();
       it != this->_blocks.cend();
       ++it) {
    idx = (it - begin);

    if (it->lru () == this->_associativity - 1) {
      break;
    }
  }

  return idx;
}

SamplerSet::SamplerBlockVectorIterator SamplerSet::begin () {
  return this->_blocks.begin();
}

SamplerSet::SamplerBlockVectorIterator SamplerSet::end () {
  return this->_blocks.end();
}

SamplerBlock& SamplerSet::operator [] (std::size_t idx) {
  if (idx >= this->size()) {
    // TODO: Error handling.
  }

  return this->_blocks[idx];
}

const SamplerBlock& SamplerSet::operator [] (std::size_t idx) const {
  if (idx >= this->_blocks.size()) {
    // TODO: Error handling.
  }

  return this->_blocks[idx];
}
