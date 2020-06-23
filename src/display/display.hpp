#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <LiquidCrystal_I2C.h>

#include "screen.hpp"

namespace ns_display {
void screenLightSettingRepresenter(int8_t value, char* row);
}

class Display {
 public:
  enum ScreenLightingSetting : int8_t {
    LIGHT_ON = 0,
    LIGHT_OFF = 1,
    LIGHT_AUTO = 2,
  };
  enum NonASCIISymbol {
    SYMBOL_DEGREE = 0x01,
  };

 public:
  Display(uint8_t lcdAddres);
  bool begin();
  void resetInterruptTimer();
  void loop();
  inline void setScreen(Screen* screen) { screen_ = screen; };

 private:
  static constexpr const uint32_t backlightAutoDisableTime_ = 20LU * 1000LU;
  static constexpr const uint8_t rowsCount_ = 2;
  static constexpr const uint8_t columnsCount_ = 16;

 private:
  LiquidCrystal_I2C lcd_;
  uint32_t lastInterruptTime_;
  Screen* screen_;

 public:  // it is really bad, but no fair to fix architecture of menu right now
  ScreenLightingSetting screenLightingSetting_;
};

#endif