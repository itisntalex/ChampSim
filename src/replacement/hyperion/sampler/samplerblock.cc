#include <iostream>
#
#include "samplerblock.hh"

using namespace Hyperion;
using namespace Hyperion::Sampler;

SamplerBlock::SamplerBlock () :
  _PC(0x0), _partialTag(0x0), _lastTrace(Features::BaseFeature::TraceType ()), _lastConfValue(0x0),
  _lru(0x0), _lastQuanta(0x0), _prefetched(false) {

}

SamplerBlock::SamplerBlock (const SamplerBlock &other) :
  _PC(other._PC), _partialTag(other._partialTag), _lastTrace(other._lastTrace),
  _lastConfValue(other._lastConfValue), _lru(other._lru),
  _lastQuanta(other._lastQuanta), _prefetched(other._prefetched) {

}

SamplerBlock::~SamplerBlock () {

}

uint64_t SamplerBlock::PC () const {
  return this->_PC;
}

uint64_t SamplerBlock::partialTag () const {
  return this->_partialTag;
}

const Features::BaseFeature::TraceType& SamplerBlock::lastTrace () const {
  return this->_lastTrace;
}

Predictor::Perceptron::OutputType SamplerBlock::lastConfValue () const {
  return this->_lastConfValue;
}

uint8_t SamplerBlock::lru () const {
  return this->_lru;
}

uint16_t SamplerBlock::lastQuanta () const {
  return this->_lastQuanta;
}

bool SamplerBlock::prefetched () const {
  return this->_prefetched;
}

void SamplerBlock::setPC (uint64_t v) {
  this->_PC = v;
}

void SamplerBlock::setPartialTag (uint64_t v) {
  this->_partialTag = v;
}

void SamplerBlock::setLastTrace (const Features::BaseFeature::TraceType& v) {
  this->_lastTrace = v;
}

void SamplerBlock::setLastConfValue (const Predictor::Perceptron::OutputType& v) {
  this->_lastConfValue = v;
}

void SamplerBlock::setLastQuanta (uint16_t v) {
  this->_lastQuanta = v;
}

void SamplerBlock::setLRU (uint8_t v) {
  this->_lru = v;
}

void SamplerBlock::setPrefeteched (bool v) {
  this->_prefetched = v;
}

void SamplerBlock::promote () {
  this->_lru = 0x0;
}

void SamplerBlock::demote () {
  this->_lru = std::min(0x11, this->_lru + 1);
}

SamplerBlock& SamplerBlock::operator = (const SamplerBlock &other) {
  this->_PC = other._PC;
  this->_partialTag = other._partialTag;
  this->_lastTrace = other._lastTrace;
  this->_lastConfValue = other._lastConfValue;
  this->_lru = other._lru;
  this->_lastQuanta = other._lastQuanta;
  this->_prefetched = other._prefetched;

  return *this;
}

bool SamplerBlock::operator == (uint64_t tag) {
  return (this->_partialTag == tag);
}
