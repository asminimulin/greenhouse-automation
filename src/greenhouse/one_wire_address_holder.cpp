#include "one_wire_address_holder.hpp"

OneWireAddressHolder::OneWireAddressHolder(const uint8_t* address)
    : address_(address) {}

size_t OneWireAddressHolder::printTo(Print& print) const {
  size_t result = 0;
  for (int i = 0; i < 8; ++i) {
    if (i) result += print.print(F(" "));
    result += print.print(address_[i], HEX);
  }
  return result;
}