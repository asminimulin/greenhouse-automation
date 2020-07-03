#include "greenhouse/greenhouse.hpp"

#include <EEPROM.h>

#include "logging/logging.hpp"

Greenhouse::Greenhouse(const GreenhouseConfig& config,
                       uint16_t settingsPosition)
    : yellowWindow_(config.openingTime, config.yellowMotorAddress),
      greenWindow_(config.openingTime, config.greenMotorAddress),
      yellowSensor_(config.yellowSensorAddress),
      greenSensor_(config.greenSensorAddress),
      outsideSensor_(config.outsideSensorAddress),
      vent_(config.ventAddress) {
  settingsPosition_ = settingsPosition;
}

bool Greenhouse::begin() {
  return yellowWindow_.begin() && greenWindow_.begin();
}

void Greenhouse::loop() {
  settings_.summerMode &= 0b1;
  yellowWindow_.loop();
  greenWindow_.loop();

  int8_t yellowTemperature = yellowSensor_.getTemperature();
  int8_t greenTemperature = greenSensor_.getTemperature();
  int8_t outsideTemperature = outsideSensor_.getTemperature();
  if (yellowTemperature == -127) {
    yellowTemperature = greenTemperature;
  }
  if (greenTemperature == -127) {
    greenTemperature = yellowTemperature;
  }
  if (yellowTemperature == -127 && greenTemperature == -127) {
    yellowTemperature = greenTemperature = outsideSensor_.getTemperature();
  }
  if (outsideTemperature == -127 && yellowTemperature == -127 &&
      greenTemperature == -127) {
    logging::error() << F("All temperature sensors are died.");
    return;
  }

  bool shouldOpenYellowWindow =
      outsideSensor_.getTemperature() > outsideMotorEnablingTemperature &&
      yellowTemperature > settings_.openingTemperature;
  shouldOpenYellowWindow &= bool(settings_.summerMode);
  shouldOpenYellowWindow |= yellowTemperature >= criticalHighTemperature;
  if (shouldOpenYellowWindow) {
    if (!yellowWindow_.isBusy() && millis() - yellowWindowStateChangedAt >
                                       settings_.temperatureInnercyDelay) {
      yellowWindow_.stepOpen(getOneStepTime());
      yellowWindowStateChangedAt = millis();
    }
  }

  bool shouldOpenGreenWindow =
      outsideSensor_.getTemperature() > outsideMotorEnablingTemperature &&
      greenTemperature > settings_.openingTemperature;
  shouldOpenGreenWindow &= bool(settings_.summerMode);
  shouldOpenGreenWindow |= greenTemperature >= criticalHighTemperature;
  if (shouldOpenGreenWindow) {
    if (!greenWindow_.isBusy() && millis() - greenWindowStateChangedAt >
                                      settings_.temperatureInnercyDelay) {
      greenWindow_.stepOpen(getOneStepTime());
      greenWindowStateChangedAt = millis();
    }
  }

  bool shouldCloseYellowWindow =
      yellowTemperature < settings_.closingTemperature &&
      yellowTemperature < criticalHighTemperature;
  shouldCloseYellowWindow |= outsideTemperature <= criticalLowTemperature;
  shouldCloseYellowWindow |= !bool(settings_.summerMode);
  if (shouldCloseYellowWindow) {
    if (!yellowWindow_.isBusy() && millis() - yellowWindowStateChangedAt >
                                       settings_.temperatureInnercyDelay) {
      yellowWindow_.stepClose(getOneStepTime());
      yellowWindowStateChangedAt = millis();
    }
  }

  bool shouldCloseGreenWindow =
      greenTemperature < settings_.closingTemperature &&
      greenTemperature < criticalHighTemperature;
  shouldCloseGreenWindow |= outsideTemperature <= criticalLowTemperature;
  shouldCloseGreenWindow |= !bool(settings_.summerMode);
  if (shouldCloseGreenWindow) {
    if (!greenWindow_.isBusy() && millis() - greenWindowStateChangedAt >
                                      settings_.temperatureInnercyDelay) {
      greenWindow_.stepClose(getOneStepTime());
      greenWindowStateChangedAt = millis();
    }
  }

  int averageTemperature = (int(yellowTemperature) + int(greenTemperature)) / 2;
  bool shouldOnVent = averageTemperature >= settings_.ventOnTemperature &&
                      ventMode_ == VENT_AUTO;
  shouldOnVent |= ventMode_ == VENT_ENABLE;
  bool shouldOffVent = ventMode_ == VENT_AUTO &&
                       averageTemperature <= settings_.ventOnTemperature - 2;
  // NOTE: 2 is histeresis value for vent ON/OFF
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
    logging::info() << F("Loading greenhouse settings");
    position += sizeof(HAS_VALID_DATA);
    EEPROM.get(position, settings_);
  }
  logging::info() << F("Using default settings");
}

void Greenhouse::saveSettings() {
  logging::info() << F("Save greenhouse settings");
  auto position = settingsPosition_;
  EEPROM.write(position, HAS_VALID_DATA);
  position += sizeof(HAS_VALID_DATA);
  EEPROM.put(position, settings_);
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

bool Greenhouse::getSummerMode() const noexcept { return settings_.summerMode; }

void Greenhouse::setSummeMode(bool enabled) noexcept {
  settings_.summerMode = enabled;
}