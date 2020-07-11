#include "esp_handler.hpp"

#include <logging.hpp>

EspHandler::EspHandler(Stream* stream, Greenhouse& greenhouse,
                       uint64_t queryTimeout)
    : queryTimeout_(queryTimeout), greenhouse_(greenhouse), stream_(stream) {}

void EspHandler::loop() {
  if (stream_->available()) {
    uint8_t command = stream_->read();
    handleCommand(command);
  }
}

void EspHandler::handleCommand(uint8_t command) {
  if (command == SET_YELLOW_SENSOR_ADDRESS) {
    setYellowSensorAddress();
  } else if (command == SET_GREEN_SENSOR_ADDRESS) {
    setGreenSensorAddress();
  } else if (command == SET_OUTSIDE_SENSOR_ADDRESS) {
    setOutsideSensorAddress();
  } else if (command == SET_YELLOW_WINDOW_ADDRESS) {
    setYellowWindowAddress();
  } else if (command == SET_GREEN_SENSOR_ADDRESS) {
    setGreenSensorAddress();
  } else if (command == GET_MEASURES) {
    getMeasures();
  } else if (command == GET_SETTINGS) {
    getSettings();
  } else if (command == SET_SETTINGS) {
    setSettings();
  } else {
    logging::warning() << F("Got unknown command from esp");
    stream_->write(ERROR);
  }
}

void EspHandler::setYellowSensorAddress() {
  constexpr size_t oneWireAddresSize = 8;
  if (!readSync(oneWireAddresSize)) {
    return;
  }
  greenhouse_.setYellowSensorAddress(buffer_);
  writeEmptyResponse(OK);
}

void EspHandler::setGreenSensorAddress() {
  constexpr size_t oneWireAddressSize = 8;
  if (!readSync(oneWireAddressSize)) return;
  greenhouse_.setGreenSensorAddress(buffer_);
  writeEmptyResponse(OK);
}

void EspHandler::setOutsideSensorAddress() {
  constexpr size_t oneWireAddressSize = 8;
  if (!readSync(oneWireAddressSize)) return;
  greenhouse_.setOutsideSensorAddress(buffer_);
  writeEmptyResponse(OK);
}

void EspHandler::setYellowWindowAddress() {
  constexpr size_t oneWireAddressSize = 8;
  if (!readSync(oneWireAddressSize)) return;
  greenhouse_.setYellowWindowAddress(buffer_);
  writeEmptyResponse(OK);
}

void EspHandler::setGreenWindowAddress() {
  constexpr size_t oneWireAddressSize = 8;
  if (!readSync(oneWireAddressSize)) return;
  greenhouse_.setGreenWindowAddress(buffer_);
  writeEmptyResponse(OK);
}

void EspHandler::getMeasures() {
  constexpr size_t messageSize = 10;
  int8_t yellowTemperature = greenhouse_.getYellowTemperature();
  int8_t greenTemperature = greenhouse_.getGreenTemperature();
  int8_t outsideTemperature = greenhouse_.getOutsideTemperature();
  uint8_t ventState = greenhouse_.isVentEnabled();
  uint8_t yellowPerCent = greenhouse_.getYellowPerCent();
  uint8_t greenPerCent = greenhouse_.getGreenPerCent();
  uint8_t blueHumidity = 0;
  uint8_t blueWateringState = 0;
  uint8_t redHumidity = 0;
  uint8_t redWateringState = 0;
  stream_->write(OK);
  stream_->write(uint8_t(messageSize));
  stream_->write(yellowTemperature);
  stream_->write(greenTemperature);
  stream_->write(outsideTemperature);
  stream_->write(ventState);
  stream_->write(yellowPerCent);
  stream_->write(greenPerCent);
  stream_->write(blueHumidity);
  stream_->write(blueWateringState);
  stream_->write(redHumidity);
  stream_->write(redWateringState);
}

void EspHandler::getSettings() {
  constexpr size_t messageSize = sizeof(ns_greenhouse::settings_t);
  auto settingsReference = greenhouse_.getSettingsReference();
  stream_->write(OK);
  stream_->write(uint8_t(messageSize));
  stream_->write(reinterpret_cast<const char*>(&settingsReference),
                 messageSize);
}

void EspHandler::setSettings() {
  constexpr size_t settingsSize = sizeof(ns_greenhouse::settings_t);
  ns_greenhouse::settings_t settings;
  if (!readSync(settingsSize)) return;
  memcpy(&settings, buffer_, settingsSize);
  greenhouse_.setSettings(settings);
  writeEmptyResponse(OK);
}

bool EspHandler::readSync(size_t length) {
  if (length >= BUFFER_SIZE_)
    logging::error() << F("Trying to read to many symbols:") << length;
  uint64_t start = millis();
  size_t ready = 0;
  while (millis() - start < queryTimeout_ && ready < length) {
    if (stream_->available()) {
      buffer_[ready++] = stream_->read();
    }
  }
  return ready == length;
}

void EspHandler::writeEmptyResponse(uint8_t status) {
  constexpr size_t emptyResponseLength = 0;
  stream_->write(status);
  stream_->write(uint8_t(emptyResponseLength));
}
