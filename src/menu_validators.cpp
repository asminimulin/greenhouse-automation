#include "menu_validators.hpp"


extern Greenhouse firstGreenhouse;
extern Greenhouse secondGreenhouse;


bool validateFirstOpeningTemperature(int8_t temp) {
    return firstGreenhouse.closingTemperature + HISTERESIS <= temp;
}


bool validateFirstClosingTemperature(int8_t temp) {
    return firstGreenhouse.openingTemperature - HISTERESIS >= temp;
}


bool validateFirstOpeningSteps(int8_t steps) {
    return steps >= 1;
}


bool validateSecondOpeningTemperature(int8_t temp) {
    return secondGreenhouse.closingTemperature + HISTERESIS <= temp;
}


bool validateSecondClosingTemperature(int8_t temp) {
    return secondGreenhouse.openingTemperature - HISTERESIS >= temp;
}


bool validateSecondOpeningSteps(int8_t steps) {
    return steps >= 1;
}
