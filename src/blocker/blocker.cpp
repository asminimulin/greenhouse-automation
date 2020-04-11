#include <Arduino.h>
#include <EEPROM.h>

#include "blocker/blocker.hpp"
#include "config.hpp"


namespace ns_blocker {


static uint32_t timePassed;
static uint32_t timeHas;
static constexpr uint32_t DAY_TIME = 1000lu * 60lu * 60lu * 24lu;
static constexpr uint32_t WEEK_TIME = 7 * DAY_TIME;
static constexpr uint32_t MONTH_TIME = 30 * DAY_TIME;


enum {
    CORRECT_DATA = 1,
    NO_DATA = 0,
};


void init() {
    uint16_t position = EEPROM_BLOCKER_TIMER_ADDRESS;
    auto dataCode = EEPROM.read(position);
    if (dataCode == CORRECT_DATA && BLOCKER_USE_EEPROM) {
        EEPROM.get(position + 1, timePassed);
    } else {
        timePassed = 0;
    }
    timeHas = BLOCKER_BLOCK_AFTER_MONTHS * MONTH_TIME + BLOCKER_BLOCK_AFTER_WEEKS * WEEK_TIME;
}


bool isBlocked() {
    return millis() + timePassed >= timeHas;
}


void updateTime() {
    uint16_t position = EEPROM_BLOCKER_TIMER_ADDRESS;
    EEPROM.write(position, CORRECT_DATA);
    EEPROM.put(position + 1, millis() + timePassed);
}

}