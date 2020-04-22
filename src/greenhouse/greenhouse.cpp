#include <EEPROM.h>

#include "logging/logging.hpp"
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
    int8_t greenTemperature = greenSensor.getTemperature();
    int8_t outsideTemperature = outsideSensor.getTemperature();
    if (yellowTemperature == 85 || yellowTemperature == -127) {
        logging::warning(F("Yellow sensor is dead"));
        yellowTemperature = greenTemperature;
    }
    if (greenTemperature == 85 || greenTemperature == -127) {
        logging::warning(F("Green sensor is dead"));
        greenTemperature = yellowTemperature;
    }
    if (yellowTemperature == -127 || greenTemperature == -127) {
        logging::error(F("Inside sensors are dead"));
        yellowTemperature = greenTemperature = outsideSensor.getTemperature();
    }
    if (yellowTemperature == -127) {
        logging::error(F("All temperature sensors are died."));
        return;
    }

    bool shouldOpenYellowWindow = outsideSensor.getTemperature() > outsideMotorEnablingTemperature && yellowTemperature > openingTemperature;
    shouldOpenYellowWindow |= yellowTemperature >= criticalHighTemperature;
    if (shouldOpenYellowWindow) {
        if (!yellowMotor.isBusy() && millis() - yellowWindowStateChangedAt > temperatureInnercyDelay) {
            logging::debug(F("Opening yellow window"));
            yellowMotor.setStateFor(OPENINIG, getOneStepTime());
            yellowWindowStateChangedAt = millis();
        }
    }

    bool shouldCloseYellowWindow = yellowTemperature < closingTemperature && yellowTemperature < criticalHighTemperature;
    shouldCloseYellowWindow |= outsideTemperature <= criticalLowTemperature;
    if (shouldCloseYellowWindow) {
        if (!yellowMotor.isBusy() && millis() - yellowWindowStateChangedAt > temperatureInnercyDelay) {
            logging::debug(F("Closing yellow window"));
            yellowMotor.setStateFor(CLOSING, getOneStepTime());
            yellowWindowStateChangedAt = millis();
        }
    }


    bool shouldOpenGreenWindow = outsideSensor.getTemperature() > outsideMotorEnablingTemperature && greenTemperature > openingTemperature;
    shouldOpenGreenWindow |= greenTemperature >= criticalHighTemperature;
    
    if (shouldOpenGreenWindow) {
        if (!greenMotor.isBusy() && millis() - greenWindowStateChangedAt > temperatureInnercyDelay) {
            logging::debug(F("Opening green window"));
            greenMotor.setStateFor(OPENINIG, getOneStepTime());
            greenWindowStateChangedAt = millis();
        }
    }

    bool shouldCloseGreenWindow = greenTemperature < closingTemperature && greenTemperature < criticalHighTemperature;
    shouldCloseGreenWindow |= outsideTemperature <= criticalLowTemperature;
    if (shouldCloseGreenWindow) {
        if (!greenMotor.isBusy() && millis() - greenWindowStateChangedAt > temperatureInnercyDelay) {
            logging::debug(F("Closing green window"));
            greenMotor.setStateFor(CLOSING, getOneStepTime());
            greenWindowStateChangedAt = millis();
        }
    }


    int averageTemperature = (int(yellowTemperature) + int(greenTemperature)) / 2;
    if (averageTemperature >= ventOnTemperature) {
        if (vent_.getState() != VentState::VENT_ON)
            vent_.setState(VentState::VENT_ON);
    } else {
        if (vent_.getState() != VentState::VENT_OFF)
            vent_.setState(VentState::VENT_OFF);
    }
}


void Greenhouse::loadSettings() {
    auto position = settingsPosition_;
    if (EEPROM.read(position) == HAS_VALID_DATA) {
        logging::info(F("Loading greenhouse settings"));
        position++;
        EEPROM.get(position, openingTemperature);
        position++;
        EEPROM.get(position, closingTemperature);
        position++;
        EEPROM.get(position, openingSteps);
    }
    logging::info(F("Using default settings"));
}


void Greenhouse::saveSettings() {
    logging::info(F("Save greenhouse settings"));
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
