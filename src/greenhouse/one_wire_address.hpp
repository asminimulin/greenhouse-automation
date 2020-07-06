#pragma once

#include <stddef.h>
#include <stdint.h>

extern const size_t length;

class IOneWireAddress {
 public:
  IOneWireAddress() noexcept = default;
  virtual uint8_t operator[](size_t index) const noexcept = 0;
  virtual uint8_t& operator[](size_t index) noexcept = 0;
  virtual ~IOneWireAddress() noexcept = default;
};

class RamOneWireAddress : public IOneWireAddress {
 public:
  RamOneWireAddress(const uint8_t* rawAddress = nullptr) noexcept;
  uint8_t operator[](size_t index) const noexcept override;
  uint8_t& operator[](size_t index) noexcept override;
  void setRawAddress(const uint8_t* address) noexcept;
  const uint8_t* getRawAddress() const noexcept;
  ~RamOneWireAddress() noexcept = default;

 private:
  static constexpr size_t length = 8;
  uint8_t raw_[length];
};