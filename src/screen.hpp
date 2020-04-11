#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <LiquidCrystal_I2C.h>

namespace ns_screen {

void loop();


void screenLightSettingRepresenter(int8_t value, char* row);


void init();


char* getWritableBuffer(uint8_t rowNumber);


void resetInterruptTimer();


enum ScreenLightSetting: int8_t {
    LIGHT_ON = 0,
    LIGHT_OFF = 1,
    LIGHT_AUTO = 2,
};


extern ScreenLightSetting screenLightSetting;

}

#endif