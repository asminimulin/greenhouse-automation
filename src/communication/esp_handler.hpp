#pragma once

#include <Stream.h>
#include <stdint.h>

#include "greenhouse/greenhouse.hpp"

/*
 * This class implements communication protocol
 * (see docs/CommunicationProtocol.md) between Arduino
 * and ESP in greenhouse controller.
 */
class EspHandler {
 public:
  /**
   * Constructor
   *
   * @param stream      Communication bus implementation between
   *                    ESP and Arduino
   * @param greenhouse  Reference to greenhouse. All queries
   *                    got on bus will be applied to this
   *                    greenhouse
   */
  EspHandler(Stream* stream, Greenhouse& greenhouse,
             uint64_t queryTimeout = 300);

  /**
   * Method that needs to be called every time in loop.
   * It is required for implementing non-intrusive
   * multitasking on greenhouse controller.
   */
  void loop();

 private:
  /**
   * Queries handling implementation
   *
   * @param command     Command got on bus
   */
  void handleCommand(uint8_t command);

  /** Queries implementation */

  void setYellowSensorAddress();
  void setGreenSensorAddress();
  void setOutsideSensorAddress();
  void setYellowWindowAddress();
  void setGreenWindowAddress();
  void setVentAddress();
  void getMeasures();
  void getSettings();
  void setSettings();

 private:
  /** Utilities */
  bool readSync(size_t length = 1);
  void writeEmptyResponse(uint8_t status);

 private:
  enum Commands : uint8_t {
    SET_YELLOW_SENSOR_ADDRESS = 0x20,
    SET_GREEN_SENSOR_ADDRESS = 0x21,
    SET_OUTSIDE_SENSOR_ADDRESS = 0x22,
    SET_YELLOW_WINDOW_ADDRESS = 0x23,
    SET_GREEN_WINDOW_ADDRESS = 0x24,
    SET_VENT_ADDRESS = 0x25,
    GET_MEASURES = '1',
    GET_SETTINGS = '2',
    SET_SETTINGS = '3',
  };

  enum ResponseCodes : uint8_t {
    OK = 0,
    ERROR = 255,
  };

 private:
  static constexpr size_t BUFFER_SIZE_ = 32;
  uint8_t buffer_[BUFFER_SIZE_];
  uint64_t queryTimeout_;
  Greenhouse& greenhouse_;
  Stream* stream_;
};
