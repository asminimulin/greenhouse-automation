#pragma once

#include <Print.h>
#include <Printable.h>
#include <stdint-gcc.h>

class OneWireAddressHolder : public Printable {
 public:
  OneWireAddressHolder(const uint8_t* address);
  size_t printTo(Print& print) const final;

 private:
  const uint8_t* address_;
};
