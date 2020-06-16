#ifndef DS2413_HPP
#define DS2413_HPP

#include <stdint-gcc.h>

class DS2413 {
 public:
  DS2413(const uint8_t* address);
  bool setState(uint8_t state);
  void setStateFor(uint8_t state, uint32_t time);
  uint8_t getState() const;
  void loop();
  bool isBusy() const;

 private:
  uint8_t address_[8];
  uint32_t actionFinishTime_;
  uint8_t previousState_ : 2;
  uint8_t state_ : 2;
  uint8_t isBusy_ : 1;
  enum {
    DS2413_FAMILY_ID = 0x3A,
    DS2413_ACCESS_READ = 0xF5,
    DS2413_ACCESS_WRITE = 0x5A,
    DS2413_ACK_SUCCESS = 0xAA,
    DS2413_ACK_ERROR = 0xFF
  };
};

#endif