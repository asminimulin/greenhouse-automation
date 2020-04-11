#include <EEPROM.h>

#include "greenhouse/greenhouse.hpp"


Greenhouse::Greenhouse(const GreenhouseConfig& config, uint16_t settingsPosition):
    yellowMotor(config.yellowMotorAddress),
    greenMotor(config.greenMotorAddress),
    yellowSensor(config.yellowSensorAddress),
    greenSensor(config.greenSensorAddress),
    outsideSensor(config.outsideSensorAddress),
    vent_(config.ventAddress) {
        settingsPosition_ = settingsPosition;
        openingSteps = 4;
        openingTemperature = 24;
        closingTemperature = 20;
}


void Greenhouse::loop() {
    yellowMotor.loop();
    greenMotor.loop();
    int8_t yellowTemperature = yellowSensor.getTemperature();
    if ((outsideSensor.getTemperature() > outsideMotorEnablingTemperature && yellowTemperature > openingTemperature) ||
            yellowTemperature >= criticalHighTemperature) {
        if (!yellowMotor.isBusy()) {
            yellowMotor.setStateFor(OPENINIG, getOneStepTime());
        }
    }
    if ((yellowTemperature < closingTemperature && yellowTemperature < criticalHighTemperature) ||
            yellowTemperature <= criticalLowTemperature) {
        if (!yellowMotor.isBusy()) {
            yellowMotor.setStateFor(CLOSING, getOneStepTime());
        }
    }

    int8_t greenTemperature = greenSensor.getTemperature();
    if ((outsideSensor.getTemperature() > outsideMotorEnablingTemperature && greenTemperature > openingTemperature) ||
            greenTemperature >= criticalHighTemperature) {
        if (!greenMotor.isBusy()) {
            greenMotor.setStateFor(OPENINIG, getOneStepTime());
        }
    }
    if ((greenTemperature < closingTemperature && greenTemperature < criticalHighTemperature) ||
            greenTemperature <= criticalLowTemperature) {
        if (!greenMotor.isBusy()) {
            greenMotor.setStateFor(CLOSING, getOneStepTime());
        }
    }

    int averageTemperature = (int(yellowTemperature) + int(greenTemperature)) / 2;
    if (averageTemperature >= ventOnTemperature) {
        vent_.setState(VentState::VENT_ON);
    } else {
        vent_.setState(VentState::VENT_OFF);
    }
}


void Greenhouse::loadSettings() {
    auto position = settingsPosition_;
    if (EEPROM.read(position) == HAS_VALID_DATA) {
        position++;
        EEPROM.get(position, openingTemperature);
        position++;
        EEPROM.get(position, closingTemperature);
        position++;
        EEPROM.get(position, openingSteps);
    }
}


void Greenhouse::saveSettings() {
    auto position = settingsPosition_;
    EEPROM.write(position, HAS_VALID_DATA);
    position++;
    EEPROM.put(position, openingTemperature);
    position++;
    EEPROM.put(position, closingTemperature);
    position++;
    EEPROM.put(position, openingSteps);

}


void Greenhouse::getTempRepresentation(int8_t temperature, char* buffer) {
    if (temperature == -127 || temperature == 85) {
        sprintf(buffer, "--");
    } else {
        sprintf(buffer, "%d", temperature);
    }
}
