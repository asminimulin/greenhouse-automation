#include "yellow_window_screen.hpp"

#include <avr/pgmspace.h>

#include "display.hpp"

YellowWindowScreen::YellowWindowScreen(Greenhouse* greenhouse)
    : greenhouse_(greenhouse) {}

void YellowWindowScreen::renderFirst() {
  char* row = getFirstBuffer();
  strcpy_P(row, reinterpret_cast<PGM_P>(F("Yellow window   ")));
}

void YellowWindowScreen::renderSecond() {
  char* row = getSecondBuffer();
  char temperatureRepresentation[5];
  temperatureToString(temperatureRepresentation,
                      greenhouse_->getYellowTemperature());
  snprintf_P(row, Screen::columns + 1,
             reinterpret_cast<PGM_P>(F("  %s  %3d%%  ")),
             temperatureRepresentation, greenhouse_->getYellowPerCent());
}

void YellowWindowScreen::temperatureToString(char* buffer, int8_t temperature) {
  if (temperature == 85) {
    sprintf_P(buffer, reinterpret_cast<PGM_P>(F("__")));
  } else if (temperature == -127) {
    sprintf_P(buffer, reinterpret_cast<PGM_P>(F("--")));
  } else {
    sprintf_P(buffer, reinterpret_cast<PGM_P>(F("%2d%cC")), temperature,
              Display::SYMBOL_DEGREE);
  }
}