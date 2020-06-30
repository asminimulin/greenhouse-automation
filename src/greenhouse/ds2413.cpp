#include "ds2413.hpp"

#include <stdint.h>

#include "global_singletons.hpp"
#include "logging/logging.hpp"
#include "one_wire_address_holder.hpp"

DS2413::DS2413(const uint8_t* address) {
  memcpy(address_, address, 8);
  isBusy_ = false;
}

bool DS2413::setState(uint8_t state) {
  if (isBusy()) {
    logging::error() << F("Tried to setState on busy ds2413");
    return false;
  }
  if (state_ == state) {
    logging::warning() << F("Setting the same state on ds2413");
    return false;
  }
  state_ = state;
  state |= 0xFC;
  OneWire* ow = getOneWire();
  ow->reset();
  ow->select(address_);
  ow->write(DS2413_ACCESS_WRITE);
  ow->write(state);
  ow->write(~state);

  bool ack = DS2413_ACK_SUCCESS == ow->read();  // 0xAA=success, 0xFF=failure
  if (ack) {
    ow->read();  // Read the status byte
  }
  ow->reset();
  if (!ack) {
    logging::debug() << F("Failed to write to ds2413");
  }
  return ack;
}

uint8_t DS2413::getState() const { return state_; }

void DS2413::loop() {
  if (!isBusy_) return;
  if (actionFinishTime_ <= millis()) {
    isBusy_ = false;
    setState(previousState_);
  }
}

bool DS2413::isBusy() const { return isBusy_; }

void DS2413::setStateFor(uint8_t state, uint32_t time) {
  if (isBusy_) return;
  actionFinishTime_ = millis() + time;
  previousState_ = state_;
  setState(state);
  isBusy_ = true;
}
