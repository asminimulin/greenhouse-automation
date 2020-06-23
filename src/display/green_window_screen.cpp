#include "green_window_screen.hpp"

#include "display.hpp"

GreenWindowScreen::GreenWindowScreen(Greenhouse* greenhouse)
    : greenhouse_(greenhouse) {}

void GreenWindowScreen::renderFirst() {
  char* row = getFirstBuffer();
  strcpy_P(row, reinterpret_cast<PGM_P>(F("Green window")));
}

void GreenWindowScreen::renderSecond() {
  char* row = getSecondBuffer();
  char temperatureRepresentation[5];
  temperatureToString(temperatureRepresentation,
                      greenhouse_->getGreenTemperature());
  snprintf_P(row, Screen::columns + 1,
             reinterpret_cast<PGM_P>(F("  %s  %3d%%  ")),
             temperatureRepresentation, greenhouse_->getGreenPerCent());
}

void GreenWindowScreen::temperatureToString(char* buffer, int8_t temperature) {
  if (temperature == 85) {
    sprintf_P(buffer, reinterpret_cast<PGM_P>(F("__")));
  } else if (temperature == -127) {
    sprintf_P(buffer, reinterpret_cast<PGM_P>(F("--")));
  } else {
    sprintf_P(buffer, reinterpret_cast<PGM_P>(F("%2d%cC")), temperature,
              Display::SYMBOL_DEGREE);
  }
}