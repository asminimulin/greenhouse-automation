#include "greenhouse/greenhouse.hpp"

#include <EEPROM.h>

#include "logging/logging.hpp"

uint8_t Greenhouse::summerMode = true;

Greenhouse::Greenhouse(const GreenhouseConfig& config,
                       uint16_t settingsPosition)
    : yellowWindow_(openingTime, config.yellowMotorAddress),
      greenWindow_(openingTime, config.greenMotorAddress),
      yellowSensor(config.yellowSensorAddress),
      greenSensor(config.greenSensorAddress),
      outsideSensor(config.outsideSensorAddress),
      vent_(config.ventAddress) {
  settingsPosition_ = settingsPosition;
  openingSteps = 6;
  openingTemperature = 24;
  closingTemperature = 20;
}

void Greenhouse::loop() {
  summerMode &= 1;
  yellowWindow_.loop();
  greenWindow_.loop();

  int8_t yellowTemperature = yellowSensor.getTemperature();
  int8_t greenTemperature = greenSensor.getTemperature();
  int8_t outsideTemperature = outsideSensor.getTemperature();
  if (yellowTemperature == -127) {
    // logging::warning(F("Yellow sensor is dead"));
    yellowTemperature = greenTemperature;
  }
  if (greenTemperature == -127) {
    // logging::warning(F("Green sensor is dead"));
    greenTemperature = yellowTemperature;
  }
  if (yellowTemperature == -127 && greenTemperature == -127) {
    // logging::error(F("Inside sensors are dead"));
    yellowTemperature = greenTemperature = outsideSensor.getTemperature();
  }
  if (outsideTemperature == -127 && yellowTemperature == -127 &&
      greenTemperature == -127) {
    logging::error(F("All temperature sensors are died."));
    return;
  }

  bool shouldOpenYellowWindow =
      outsideSensor.getTemperature() > outsideMotorEnablingTemperature &&
      yellowTemperature > openingTemperature;
  shouldOpenYellowWindow |= yellowTemperature >= criticalHighTemperature;
  shouldOpenYellowWindow &= bool(summerMode);
  if (shouldOpenYellowWindow) {
    if (!yellowWindow_.isBusy() &&
        millis() - yellowWindowStateChangedAt > temperatureInnercyDelay) {
      logging::debug(F("stepOpen yellow window"));
      yellowWindow_.stepOpen(getOneStepTime());
      yellowWindowStateChangedAt = millis();
    }
  }

  bool shouldOpenGreenWindow =
      outsideSensor.getTemperature() > outsideMotorEnablingTemperature &&
      greenTemperature > openingTemperature;
  shouldOpenGreenWindow |= greenTemperature >= criticalHighTemperature;
  shouldOpenGreenWindow &= bool(summerMode);
  if (shouldOpenGreenWindow) {
    if (!greenWindow_.isBusy() &&
        millis() - greenWindowStateChangedAt > temperatureInnercyDelay) {
      logging::debug(F("stepOpen green window"));
      greenWindow_.stepOpen(getOneStepTime());
      greenWindowStateChangedAt = millis();
    }
  }

  bool shouldCloseYellowWindow = yellowTemperature < closingTemperature &&
                                 yellowTemperature < criticalHighTemperature;
  shouldCloseYellowWindow |= outsideTemperature <= criticalLowTemperature;
  shouldCloseYellowWindow |= !bool(summerMode);
  if (shouldCloseYellowWindow) {
    if (!yellowWindow_.isBusy() &&
        millis() - yellowWindowStateChangedAt > temperatureInnercyDelay) {
      logging::debug(F("stepClose yellow window"));
      yellowWindow_.stepClose(getOneStepTime());
      yellowWindowStateChangedAt = millis();
    }
  }

  bool shouldCloseGreenWindow = greenTemperature < closingTemperature &&
                                greenTemperature < criticalHighTemperature;
  shouldCloseGreenWindow |= outsideTemperature <= criticalLowTemperature;
  shouldCloseGreenWindow |= !bool(summerMode);
  if (shouldCloseGreenWindow) {
    if (!greenWindow_.isBusy() &&
        millis() - greenWindowStateChangedAt > temperatureInnercyDelay) {
      logging::debug(F("stepClose green window"));
      greenWindow_.stepClose(getOneStepTime());
      greenWindowStateChangedAt = millis();
    }
  }

  int averageTemperature = (int(yellowTemperature) + int(greenTemperature)) / 2;
  if (averageTemperature >= ventOnTemperature) {
    if (vent_.getState() != VentState::VENT_ON)
      vent_.setState(VentState::VENT_ON);
  } else if (averageTemperature <=
             ventOnTemperature -
                 2) {  // NOTE: 2 is histeresis value for vent ON/OFF
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
    position += sizeof(openingSteps);
    EEPROM.get(position, summerMode);
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
  position += sizeof(openingSteps);
  EEPROM.put(position, summerMode);
}

void Greenhouse::getTempRepresentation(int8_t temperature, char* buffer) {
  if (temperature == -127) {
    sprintf(buffer, "--");
  } else if (temperature == 85) {
    sprintf(buffer, "__");
  } else {
    sprintf(buffer, "%d", temperature);
  }
}
