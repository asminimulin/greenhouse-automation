#include "greenhouse/greenhouse.hpp"

#include <EEPROM.h>

#include "logging/logging.hpp"
#include "logging/logging2.hpp"

Greenhouse::Greenhouse(const GreenhouseConfig& config,
                       uint16_t settingsPosition)
    : yellowWindow_(config.openingTime, config.yellowMotorAddress),
      greenWindow_(config.openingTime, config.greenMotorAddress),
      yellowSensor_(config.yellowSensorAddress),
      greenSensor_(config.greenSensorAddress),
      outsideSensor_(config.outsideSensorAddress),
      vent_(config.ventAddress),
      openingTime(config.openingTime),
      temperatureInnercyDelay(config.temperatureInnercyDelay) {
  settingsPosition_ = settingsPosition;
  openingSteps = 6;
  openingTemperature = 24;
  closingTemperature = 20;
  summerMode = true;
  ventMode_ = VENT_AUTO;
}

bool Greenhouse::begin() {
  return yellowWindow_.begin() && greenWindow_.begin();
}

void Greenhouse::loop() {
  summerMode &= 1;
  yellowWindow_.loop();
  greenWindow_.loop();

  int8_t yellowTemperature = yellowSensor_.getTemperature();
  int8_t greenTemperature = greenSensor_.getTemperature();
  int8_t outsideTemperature = outsideSensor_.getTemperature();
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
    yellowTemperature = greenTemperature = outsideSensor_.getTemperature();
  }
  if (outsideTemperature == -127 && yellowTemperature == -127 &&
      greenTemperature == -127) {
    logging::error(F("All temperature sensors are died."));
    return;
  }

  bool shouldOpenYellowWindow =
      outsideSensor_.getTemperature() > outsideMotorEnablingTemperature &&
      yellowTemperature > openingTemperature;
  shouldOpenYellowWindow &= bool(summerMode);
  shouldOpenYellowWindow |= yellowTemperature >= criticalHighTemperature;
  if (shouldOpenYellowWindow) {
    if (!yellowWindow_.isBusy() &&
        millis() - yellowWindowStateChangedAt > temperatureInnercyDelay) {
      yellowWindow_.stepOpen(getOneStepTime());
      yellowWindowStateChangedAt = millis();
    }
  }

  bool shouldOpenGreenWindow =
      outsideSensor_.getTemperature() > outsideMotorEnablingTemperature &&
      greenTemperature > openingTemperature;
  shouldOpenGreenWindow &= bool(summerMode);
  shouldOpenGreenWindow |= greenTemperature >= criticalHighTemperature;
  if (shouldOpenGreenWindow) {
    if (!greenWindow_.isBusy() &&
        millis() - greenWindowStateChangedAt > temperatureInnercyDelay) {
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
      greenWindow_.stepClose(getOneStepTime());
      greenWindowStateChangedAt = millis();
    }
  }

  int averageTemperature = (int(yellowTemperature) + int(greenTemperature)) / 2;
  bool shouldOnVent =
      averageTemperature >= ventOnTemperature && ventMode_ == VENT_AUTO;
  shouldOnVent |= ventMode_ == VENT_ENABLE;
  bool shouldOffVent =
      ventMode_ == VENT_AUTO && averageTemperature <= ventOnTemperature - 2;
  // NOTE: 2 is histeresis value for vent ON/OFF
  // logging2::debug() << F("vent mode =") << int(ventMode_);
  shouldOffVent |= ventMode_ == VENT_DISABLE;
  if (shouldOnVent) {
    if (vent_.getState() != VentState::VENT_ON)
      vent_.setState(VentState::VENT_ON);
  } else if (shouldOffVent) {
    if (vent_.getState() != VentState::VENT_OFF)
      vent_.setState(VentState::VENT_OFF);
  }
}

void Greenhouse::loadSettings() {
  auto position = settingsPosition_;
  if (EEPROM.read(position) == HAS_VALID_DATA) {
    logging::info(F("Loading greenhouse settings"));
    position += sizeof(HAS_VALID_DATA);
    EEPROM.get(position, openingTemperature);
    position += sizeof(openingTemperature);
    EEPROM.get(position, closingTemperature);
    position += sizeof(closingTemperature);
    EEPROM.get(position, openingSteps);
    position += sizeof(openingSteps);
    EEPROM.get(position, summerMode);
    position += sizeof(summerMode);
    EEPROM.get(position, ventMode_);
    position += sizeof(ventMode_);
  }
  logging::info(F("Using default settings"));
}

void Greenhouse::saveSettings() {
  logging::info(F("Save greenhouse settings"));
  auto position = settingsPosition_;
  EEPROM.write(position, HAS_VALID_DATA);
  position += sizeof(HAS_VALID_DATA);
  EEPROM.put(position, openingTemperature);
  position += sizeof(openingTemperature);
  EEPROM.put(position, closingTemperature);
  position += sizeof(closingTemperature);
  EEPROM.put(position, openingSteps);
  position += sizeof(openingSteps);
  EEPROM.put(position, summerMode);
  position += sizeof(summerMode);
  EEPROM.put(position, ventMode_);
  position += sizeof(ventMode_);
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
