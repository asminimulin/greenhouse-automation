#ifndef MENU_VALIDATORS_HPP
#define MENU_VALIDATORS_HPP

#include "greenhouse/greenhouse.hpp"


extern Greenhouse firstGreenhouse;


// namespace ns_first_greenhouse_validators {

bool validateFirstOpeningTemperature(int8_t temp);
bool validateFirstClosingTemperature(int8_t temp);
bool validateFirstOpeningSteps(int8_t steps);
bool validateSecondOpeningTemperature(int8_t temp);
bool validateSecondClosingTemperature(int8_t temp);
bool validateSecondOpeningSteps(int8_t steps);
// }



extern Greenhouse secondGreenhouse;

#endif