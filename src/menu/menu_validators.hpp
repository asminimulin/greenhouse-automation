#ifndef MENU_VALIDATORS_HPP
#define MENU_VALIDATORS_HPP

#include "greenhouse/greenhouse.hpp"

extern Greenhouse greenhouse;

// namespace ns_first_greenhouse_validators {

bool validateOpeningTemperature(int8_t temp);
bool validateClosingTemperature(int8_t temp);
bool validateOpeningSteps(int8_t steps);
// }

extern Greenhouse secondGreenhouse;

#endif