#include <Arduino.h>
#include <EEPROM.h>

#include "blocker/blocker.hpp"
#include "config.hpp"


namespace ns_blocker {


static uint32_t timePassed;
static uint32_t timeHas;
static constexpr uint32_t MINUTE_TIME = 60lu * 1000lu;
static constexpr uint32_t DAY_TIME = MINUTE_TIME * 60lu * 24lu;
static constexpr uint32_t WEEK_TIME = 7 * DAY_TIME;
static constexpr uint32_t MONTH_TIME = 30 * DAY_TIME;

// we have 1e5 writes on EEPROM register with garantee
// to return all money we need get rent payment 5 times
// it means we have 1e5 / 10 = 10000 write operations per month
// 10000 / 30 = 333 write operations per day
// 333 / 24 = 15 operations per hour
// 60 / 15 = 2 mintes - interval between writes
static constexpr uint32_t WRITE_INTERVAL = MINUTE_TIME * 2;

enum : uint8_t {
    CORRECT_DATA = 1,
    NO_DATA = 0,
};


void init() {
    uint16_t position = EEPROM_BLOCKER_TIMER_ADDRESS;
    auto dataCode = EEPROM.read(position);
    if (BLOCKER_USE_EEPROM) {
        if (dataCode == CORRECT_DATA) {
            logging::info(F("Blocker: use saved to EEPROM data"));
            EEPROM.get(position + 1, timePassed);
        } else {
            logging::info(F("Blocker: do not use EEPROM data"));
            timePassed = 0;
        }
    } else {
        logging::info(F("Blocker: do not use EEPROM data"));
        EEPROM.write(position, NO_DATA);
    }
    timeHas = BLOCKER_BLOCK_AFTER_MONTHS * MONTH_TIME +
              BLOCKER_BLOCK_AFTER_WEEKS * WEEK_TIME + 
              MINUTE_TIME * BLOCKER_BLOCK_AFTER_MINUTES;
}


bool isBlocked() {
    bool result = millis() + timePassed >= timeHas;
    if (result) {
        logging::info(F("Blocker: System is blocked"));
    } else {
        logging::info(F("Blocker: System is still not blocked"));
    }
    return result;
}


void updateTime() {
    static uint32_t nextWriteTimestamp = WRITE_INTERVAL;
    uint16_t position = EEPROM_BLOCKER_TIMER_ADDRESS;
    if (millis() > nextWriteTimestamp) {
        EEPROM.put(position + 1, static_cast<uint32_t>(millis() + timePassed));
        nextWriteTimestamp = millis() + WRITE_INTERVAL;
    }
}


uint32_t getRemainingTime() {
    return timeHas - (timePassed + millis());
}

}