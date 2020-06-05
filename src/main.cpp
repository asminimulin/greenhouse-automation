#include <Arduino.h>

#include <blocker/blocker.hpp>
#include <one_wire_driver/one_wire_driver.hpp>
#include <idle.hpp>
#include <greenhouse/greenhouse.hpp>
#include "config.hpp"
#include "encoder/encoder.hpp"
#include "menu/menu.hpp"
#include "menu/menu_validators.hpp"
#include "screen/screen.hpp"
#include "logging/logging.hpp"
#include "connector/serial_connector.hpp"


void buildFirstGreenhouseMenu();
void buildSecondGreenhouseMenu();
void buildScreenMenu();
void buildSummerModeMenu();


Greenhouse buildFirstGreenhouse();
Greenhouse buildSecondGreenhouse();


Greenhouse firstGreenhouse = buildFirstGreenhouse();
Greenhouse secondGreenhouse = buildSecondGreenhouse();


void espHandle(Stream* stream);
serial_connector::SerialConnector espConnector(&Serial, espHandle);


void refreshScreen();


void skipEspTrashWriting();


void notifySystemBlocked();


void setup() {
  Serial.begin(57600);

  skipEspTrashWriting();

  if (ENABLE_DEBUG_OUTPUT) {
    logging::setup(logging::DEBUG, &Serial);
  } else {
    logging::setup(logging::NOTHING, &Serial);
  }

  ns_blocker::init();
  if (ns_blocker::isBlocked()) {
    notifySystemBlocked();
    idle();
  }

  initOneWire();

  firstGreenhouse.loadSettings();
  if (HAS_SECOND_GREENHOUSE) secondGreenhouse.loadSettings();


  ns_ds18b20::init();
  logging::info(F("ds18b20 is ready ready"));

  ns_menu::init();
  logging::info(F("menu is ready"));

  ns_screen::init();
  logging::info(F("screen is ready"));

  ns_encoder::init();
  logging::info(F("encoder is ready"));

  buildFirstGreenhouseMenu();
  if (HAS_SECOND_GREENHOUSE) buildSecondGreenhouseMenu();
  buildScreenMenu();
  buildSummerModeMenu();
  logging::info(F("Greenhouses are ready"));

  logging::info(F("Setup successfully"));
}


void loop() {
  if (ns_blocker::isBlocked()) {
    notifySystemBlocked();
    idle();
  }
  firstGreenhouse.loop();
  if (HAS_SECOND_GREENHOUSE) secondGreenhouse.loop();
  refreshScreen();
  ns_screen::loop();
  espConnector.loop();
  ns_blocker::updateTime();
}


Greenhouse buildFirstGreenhouse() {
  GreenhouseConfig config;
  memcpy(config.yellowSensorAddress,   FIRST_GREENHOUSE_YELLOW_SENSOR_ADDRESS, 8);
  memcpy(config.greenSensorAddress,    FIRST_GREENHOUSE_GREEN_SENSOR_ADDRESS, 8);
  memcpy(config.outsideSensorAddress,  FIRST_GREENHOUSE_OUTSIDE_SENSOR_ADDRESS, 8);
  memcpy(config.yellowMotorAddress,    FIRST_GREENHOUSE_YELLOW_MOTOR_ADDRESS, 8);
  memcpy(config.greenMotorAddress,     FIRST_GREENHOUSE_GREEN_MOTOR_ADDRESS, 8);
  memcpy(config.ventAddress,           FIRST_GREENHOUSE_VENT_ADDRESS, 8);
  return Greenhouse(config, EEPROM_FIRST_GREENHOUSE_PROPERTIES_ADDRESS);
}


Greenhouse buildSecondGreenhouse() {
  GreenhouseConfig config;
  memcpy(config.yellowSensorAddress,   SECOND_GREENHOUSE_YELLOW_SENSOR_ADDRESS, 8);
  memcpy(config.greenSensorAddress,    SECOND_GREENHOUSE_GREEN_SENSOR_ADDRESS, 8);
  memcpy(config.outsideSensorAddress,  SECOND_GREENHOUSE_OUTSIDE_SENSOR_ADDRESS, 8);
  memcpy(config.yellowMotorAddress,    SECOND_GREENHOUSE_YELLOW_MOTOR_ADDRESS, 8);
  memcpy(config.greenMotorAddress,     SECOND_GREENHOUSE_GREEN_MOTOR_ADDRESS, 8);
  memcpy(config.ventAddress,           SECOND_GREENHOUSE_VENT_ADDRESS, 8);
  return Greenhouse(config, EEPROM_SECOND_GREENHOUSE_PROPERTIES_ADDRESS);
}


void ns_encoder::increment() {
  ns_menu::handleIncrement();
  ns_screen::resetInterruptTimer();
}


void ns_encoder::decrement() {
  ns_menu::handleDecrement();
  ns_screen::resetInterruptTimer();
}


void ns_encoder::incrementPress() {
  ns_menu::handleIncrementPress();
  ns_screen::resetInterruptTimer();
}


void ns_encoder::decrementPress() {
  ns_menu::handleDecrementPress();
  ns_screen::resetInterruptTimer();
}


void refreshScreen() {
  char buf[3][3];
  if (!ns_menu::isEnabled()) {
    char* firstRow = ns_screen::getWritableBuffer(0);
    char* secondRow = ns_screen::getWritableBuffer(1);
    Greenhouse::getTempRepresentation(firstGreenhouse.getYellowTemperature(), buf[0]);
    Greenhouse::getTempRepresentation(firstGreenhouse.getGreenTemperature(), buf[1]);
    Greenhouse::getTempRepresentation(firstGreenhouse.getOutsideTemperature(), buf[2]);
    sprintf(firstRow, "Y:%s G:%s Out:%s", buf[0], buf[1], buf[2]);
    Greenhouse::getTempRepresentation(secondGreenhouse.getYellowTemperature(), buf[0]);
    Greenhouse::getTempRepresentation(secondGreenhouse.getGreenTemperature(), buf[1]);
    sprintf(secondRow, "Y:%s G:%s Sum:%s", buf[0], buf[1], (firstGreenhouse.getSummerMode() ? "ON" : "OFF"));
  } else {
    char *firstRow = ns_screen::getWritableBuffer(0);
    char *secondRow = ns_screen::getWritableBuffer(1);
    ns_menu::renderMenu(firstRow, secondRow);
  }

}


void buildFirstGreenhouseMenu() {
  MenuItem item;
  item.parent = ns_menu::getRoot();
  item.name = F("Greenhouse 1");
  item.isLeaf = false;
  auto rootId = ns_menu::addItem(item);

  item.name = F("Opening Temp");
  item.parent = rootId;
  item.value = &firstGreenhouse.openingTemperature;
  item.validator = validateFirstOpeningTemperature;
  item.isLeaf = true;
  ns_menu::addItem(item);
  
  item.name = F("Closing Temp");
  item.parent = rootId;
  item.value = &firstGreenhouse.closingTemperature;
  item.isLeaf = true;
  item.validator = validateFirstClosingTemperature;
  ns_menu::addItem(item);

  item.name = F("Opening steps");
  item.parent = rootId;
  item.value = reinterpret_cast<int8_t*>(&firstGreenhouse.openingSteps);
  item.isLeaf = true;
  item.validator = validateFirstOpeningSteps;
  ns_menu::addItem(item);
}


void buildSecondGreenhouseMenu() {
  MenuItem item;
  item.parent = ns_menu::getRoot();
  item.name = F("Greenhouse 2");
  item.isLeaf = false;
  auto rootId = ns_menu::addItem(item);

  item.name = F("Opening Temp");
  item.parent = rootId;
  item.value = &secondGreenhouse.openingTemperature;
  item.isLeaf = true;
  item.validator = validateSecondOpeningTemperature;
  ns_menu::addItem(item);
  
  item.name = F("Closing Temp");
  item.parent = rootId;
  item.value = &secondGreenhouse.closingTemperature;
  item.isLeaf = true;
  item.validator = validateSecondClosingTemperature;
  ns_menu::addItem(item);

  item.name = F("Opening steps");
  item.parent = rootId;
  item.value = reinterpret_cast<int8_t*>(&secondGreenhouse.openingSteps);
  item.isLeaf = true;
  item.validator = validateSecondOpeningSteps;
  ns_menu::addItem(item);
}


void buildScreenMenu() {
  MenuItem item;
  item.parent = ns_menu::getRoot();
  item.name = F("Screen light");
  item.isLeaf = true;
  item.value = reinterpret_cast<int8_t*>(&ns_screen::screenLightSetting);
  item.hasCustomRepresenation = true;
  item.represent = ns_screen::screenLightSettingRepresenter;
  ns_menu::addItem(item);
}


void repsentSummerMode(int8_t value, char* buffer) {
  if ((value & 1) == 0) {
    strcpy_P(buffer, reinterpret_cast<const char*>(F("OFF")));
  } else {
    strcpy_P(buffer, reinterpret_cast<const char*>(F("ON")));
  }
}


void buildSummerModeMenu() {
  MenuItem item;
  item.parent = ns_menu::getRoot();
  item.name = F("Summer mode");
  item.isLeaf = true;
  item.value = reinterpret_cast<int8_t*>(&Greenhouse::summerMode);
  item.hasCustomRepresenation = true;
  item.represent = repsentSummerMode;
  ns_menu::addItem(item);
}


enum EspCommandInterfaces: uint8_t {
  COMMAND_GET_MEASURES = '1',
  COMMAND_GET_SETTINGS = '2',
};
enum StatusCode: uint8_t {
  OK = 0,
  ERROR = 1,
};
void espHandle(Stream* stream) {
  auto command = stream->read(); // we for sure know that stream is not empty (see implementation of SerialConnector)
  if (command == COMMAND_GET_MEASURES) {
    auto ytemp1 = static_cast<uint8_t>(firstGreenhouse.getYellowTemperature());
    auto gtemp1 = static_cast<uint8_t>(firstGreenhouse.getGreenTemperature());
    auto otemp = static_cast<uint8_t>(firstGreenhouse.getOutsideTemperature());
    auto ytemp2 = static_cast<uint8_t>(secondGreenhouse.getYellowTemperature());
    auto gtemp2 = static_cast<uint8_t>(secondGreenhouse.getGreenTemperature());
    stream->write(OK);
    uint8_t messageSize = 19;  // measures size
    stream->write(uint8_t(messageSize));
    stream->write(otemp);

    stream->write(ytemp1);
    stream->write(gtemp1);
    stream->write(uint8_t(firstGreenhouse.getVentStatus()));
    stream->write(firstGreenhouse.getYellowPerCent()); // yellow window per cent
    stream->write(firstGreenhouse.getGreenPerCent()); // green window per cent
    stream->write(uint8_t(0)); // blue humidity
    stream->write(uint8_t(false)); // blue watering status
    stream->write(uint8_t(0)); // red humidity
    stream->write(uint8_t(false)); // red watering status
    
    stream->write(ytemp2);
    stream->write(gtemp2);
    stream->write(uint8_t(secondGreenhouse.getVentStatus()));
    stream->write(secondGreenhouse.getYellowPerCent()); // yellow window per cent
    stream->write(secondGreenhouse.getGreenPerCent()); // green window per cent
    stream->write(uint8_t(0)); // blue humidity
    stream->write(uint8_t(false)); // blue watering status
    stream->write(uint8_t(0)); // red humidity
    stream->write(uint8_t(false)); // red watering status

  } else if (command == COMMAND_GET_SETTINGS) {
    stream->write(OK);
    uint8_t messageSize = 3 * 2;  // settings size
    stream->write(uint8_t(messageSize));

    stream->write(uint8_t(firstGreenhouse.getOpeningTemperature()));
    stream->write(uint8_t(firstGreenhouse.getClosingTemperature()));
    stream->write(firstGreenhouse.getStepsCount());

    stream->write(uint8_t(secondGreenhouse.getOpeningTemperature()));
    stream->write(uint8_t(secondGreenhouse.getClosingTemperature()));
    stream->write(secondGreenhouse.getStepsCount());
  } else {
    /* Unsupported comand -> do nothing */
    logging::error(F("Unsupported command"));
  }
}


void skipEspTrashWriting() {
  delay(3000);
  while (Serial.available()) {
    Serial.read();
  }
}


void notifySystemBlocked() {
  logging::info(F("Testing time finished. System stopped"));
  const char* message[2];
  message[0] = reinterpret_cast<const char*>(F("   Stop test   "));
  message[1] = reinterpret_cast<const char*>(F("   Time is up  "));
  char* line[2] = {ns_screen::getWritableBuffer(0),
                   ns_screen::getWritableBuffer(1)};
  for (int row = 0; row < 2; ++row) {
    for (int i = 0; ; ++i) {
      char c = pgm_read_byte(message[row] + i);
      line[row][i] = c;
      if (c == '\0') break;
    }
  }
  ns_screen::loop();
}
