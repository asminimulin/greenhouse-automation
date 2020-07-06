#include "one_wire_address.hpp"

#include <string.h>

RamOneWireAddress::RamOneWireAddress(const uint8_t* rawAddress) noexcept
    : IOneWireAddress() {
  if (nullptr != rawAddress) {
    memcpy(raw_, rawAddress, length);
  } else {
    for (size_t i = 0; i < length; ++i) {
      raw_[i] = 0;
    }
  }
}

uint8_t RamOneWireAddress::operator[](size_t index) const noexcept {
  return raw_[index];
}

uint8_t& RamOneWireAddress::operator[](size_t index) noexcept {
  return raw_[index];
}

void RamOneWireAddress::setRawAddress(const uint8_t* address) noexcept {
  memcpy(raw_, address, length);
}

const uint8_t* RamOneWireAddress::getRawAddress() const noexcept {
  return raw_;
}
