#include <stdint.h>


#include "ds2413.hpp"
#include "one_wire_driver/one_wire_driver.hpp"
#include "logging/logging.hpp"

DS2413::DS2413(const uint8_t* address) {
    memcpy(address_, address, 8);
    isBusy_ = false;
}


void DS2413::setState(uint8_t state) {
    if (isBusy()) {
        logging::error(F("Tried to setState on busy ds2413"));
        return;
    }
    if (state_ == state) {
        logging::warning(F("Setting the same state on ds2413"));
        return;
    }
    logging::debug(F("ds2413 set state:"));
    logging::debug(int(state));
    state_ = state;
    state |= 0xFC;
    OneWire* ow = getOneWire();
    ow->reset();
    ow->select(address_);
    ow->write(DS2413_ACCESS_WRITE);
    ow->write(state);
    ow->write(~state);
    ow->reset();
}


uint8_t DS2413::getState() const {
    return state_;
}

void DS2413::loop() {
    if (!isBusy_) return;
    if (actionFinishTime_ <= millis()) {
        isBusy_ = false;
        setState(previousState_);
    }
}

bool DS2413::isBusy() const {
    return isBusy_;
}

void DS2413::setStateFor(uint8_t state, uint32_t time) {
    if (isBusy_) return;
    actionFinishTime_ = millis() + time;
    previousState_ = state_;
    setState(state);
    isBusy_ = true;
}
