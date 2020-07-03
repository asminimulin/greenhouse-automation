#include "menu_validators.hpp"

extern Greenhouse greenhouse;
extern Greenhouse secondGreenhouse;

bool validateOpeningTemperature(int8_t temp) {
  return greenhouse.settings_.closingTemperature + HISTERESIS <= temp;
}

bool validateClosingTemperature(int8_t temp) {
  return greenhouse.settings_.openingTemperature - HISTERESIS >= temp;
}

bool validateOpeningSteps(int8_t steps) { return steps >= 1; }
