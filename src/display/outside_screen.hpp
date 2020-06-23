#pragma once

#include "display.hpp"
#include "greenhouse/greenhouse.hpp"
#include "screen.hpp"

class OutsideScreen : public Screen {
 public:
  inline OutsideScreen(Greenhouse* greenhouse) : greenhouse_(greenhouse) {}
  inline void renderFirst() final {
    char* row = getFirstBuffer();
    strcpy_P(row, reinterpret_cast<PGM_P>(F("Outside")));
  }
  inline void renderSecond() final {
    char* row = getSecondBuffer();
    int8_t temperature = greenhouse_->getOutsideTemperature();
    char buffer[5];
    temperatureToString(buffer, temperature);
    sprintf_P(row, reinterpret_cast<PGM_P>(F("  %s")), buffer);
  }

  inline void temperatureToString(char* buffer, int8_t temperature) {
    if (temperature == 85) {
      sprintf_P(buffer, reinterpret_cast<PGM_P>(F("__")));
    } else if (temperature == -127) {
      sprintf_P(buffer, reinterpret_cast<PGM_P>(F("--")));
    } else {
      sprintf_P(buffer, reinterpret_cast<PGM_P>(F("%2d%cC")), temperature,
                Display::SYMBOL_DEGREE);
    }
  }
  inline ~OutsideScreen() {}

 private:
  Greenhouse* greenhouse_;
};