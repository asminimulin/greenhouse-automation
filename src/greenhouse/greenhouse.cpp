#include "greenhouse/greenhouse.hpp"

#include <EEPROM.h>

#include <logging.hpp>

#include "config.hpp"
#include "one_wire_address.hpp"

Greenhouse::Greenhouse(const GreenhouseAddresses& addresses,
                       uint16_t settingsPosition)
    : yellowWindow_(OPENING_TIME, addresses.yellowWindowAddress),
      greenWindow_(OPENING_TIME, addresses.greenWindowAddress),
      yellowSensor_(addresses.yellowSensorAddress),
      greenSensor_(addresses.greenSensorAddress),
      outsideSensor_(addresses.outsideSensorAddress),
      vent_(addresses.ventAddress) {
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

int8_t Greenhouse::getOutsideTemperature() noexcept {
  return outsideSensor_.getTemperature();
}

int8_t Greenhouse::getYellowTemperature() noexcept {
  return yellowSensor_.getTemperature();
}

int8_t Greenhouse::getGreenTemperature() noexcept {
  return greenSensor_.getTemperature();
}

bool Greenhouse::isVentEnabled() const noexcept {
  return vent_.getState() == VENT_ON;
}

uint8_t Greenhouse::getYellowPerCent() const noexcept {
  return yellowWindow_.getPerCent();
}

uint8_t Greenhouse::getGreenPerCent() const noexcept {
  return greenWindow_.getPerCent();
}

bool Greenhouse::getSummerMode() const noexcept { return settings_.summerMode; }

void Greenhouse::setSummeMode(bool enabled) noexcept {
  settings_.summerMode = enabled;
}

void Greenhouse::setOpeningTemperature(int8_t openingTemperature) noexcept {
  settings_.openingTemperature = openingTemperature;
}

int8_t Greenhouse::getOpeningTemperature() const noexcept {
  return settings_.openingTemperature;
}

void Greenhouse::setClosingTemperature(int8_t closingTemperature) noexcept {
  settings_.closingTemperature = closingTemperature;
}

int8_t Greenhouse::getClosingTemperature() const noexcept {
  return settings_.closingTemperature;
}

void Greenhouse::setStepsCount(uint8_t stepsCount) noexcept {
  settings_.stepsCount = stepsCount;
}

uint8_t Greenhouse::getStepsCount() const noexcept {
  return settings_.stepsCount;
}

const Greenhouse::settings_t& Greenhouse::getSettingsReference()
    const noexcept {
  return settings_;
}

Greenhouse::settings_t Greenhouse::getSettings() const noexcept {
  return settings_;
}

void Greenhouse::setSettings(const settings_t& settings) noexcept {
  settings_ = settings;
  saveSettings();
}

void Greenhouse::setYellowSensorAddress(uint8_t* address) {
  yellowSensor_.setAddress(address);
  saveAddressesToEEPROM();
}

void Greenhouse::setGreenSensorAddress(uint8_t* address) {
  greenSensor_.setAddress(address);
  saveAddressesToEEPROM();
}

void Greenhouse::setOutsideSensorAddress(uint8_t* address) {
  outsideSensor_.setAddress(address);
  saveAddressesToEEPROM();
}

void Greenhouse::setYellowWindowAddress(uint8_t* address) {
  yellowWindow_.setAddress(address);
}

void Greenhouse::setGreenWindowAddress(uint8_t* address) {
  greenWindow_.setAddress(address);
  saveAddressesToEEPROM();
}

void Greenhouse::setVentAddress(uint8_t* address) {
  vent_.setAddress(address);
  saveAddressesToEEPROM();
}

bool Greenhouse::loadAddressesFromEEPROM() {
  int position = ONE_WIRE_ADDRESSES_EEPROM_POSITION;
  uint8_t protectionValue = EEPROM.read(position);
  if (protectionValue == ONE_WIRE_ADDRESSES_EEPROM_PROTECTION_VALUE) {
    position += sizeof(ONE_WIRE_ADDRESSES_EEPROM_PROTECTION_VALUE);
    RamOneWireAddress address;

    EEPROM.get(position, address);
    yellowSensor_.setAddress(address.getRawAddress());
    position += sizeof(address);

    EEPROM.get(position, address);
    greenSensor_.setAddress(address.getRawAddress());
    position += sizeof(address);

    EEPROM.get(position, address);
    outsideSensor_.setAddress(address.getRawAddress());
    position += sizeof(address);

    EEPROM.get(position, address);
    yellowWindow_.setAddress(address.getRawAddress());
    position += sizeof(address);

    EEPROM.get(position, address);
    greenWindow_.setAddress(address.getRawAddress());
    position += sizeof(address);

    EEPROM.get(position, address);
    vent_.setAddress(address.getRawAddress());

    return true;
  }
  return false;
}

void Greenhouse::saveAddressesToEEPROM() {
  int position = ONE_WIRE_ADDRESSES_EEPROM_POSITION;
  EEPROM.write(position, ONE_WIRE_ADDRESSES_EEPROM_PROTECTION_VALUE);
  position += sizeof(ONE_WIRE_ADDRESSES_EEPROM_PROTECTION_VALUE);
  RamOneWireAddress address;

  address.setRawAddress(yellowSensor_.getAddress());
  EEPROM.put(position, address);
  position += sizeof(address);

  address.setRawAddress(greenSensor_.getAddress());
  EEPROM.put(position, address);
  position += sizeof(address);

  address.setRawAddress(outsideSensor_.getAddress());
  EEPROM.put(position, address);
  position += sizeof(address);

  address.setRawAddress(yellowWindow_.getAddress());
  EEPROM.put(position, address);
  position += sizeof(address);

  address.setRawAddress(greenWindow_.getAddress());
  EEPROM.get(position, address);
  position += sizeof(address);

  address.setRawAddress(vent_.getAddress());
  EEPROM.put(position, address);
}

uint32_t Greenhouse::getOneStepTime() const noexcept {
  return settings_.openingTime / settings_.stepsCount;
}
