#include <Arduino.h>

#include <blocker/blocker.hpp>
#include <one_wire_driver.hpp>
#include <idle.hpp>
#include <greenhouse/greenhouse.hpp>
#include "config.hpp"
#include "encoder.hpp"
#include "menu.hpp"
#include "screen.hpp"
#include "menu_validators.hpp"



void buildFirstGreenhouseMenu();
void buildSecondGreenhouseMenu();
void buildScreenMenu();


Greenhouse buildFirstGreenhouse();
Greenhouse buildSecondGreenhouse();


Greenhouse firstGreenhouse = buildFirstGreenhouse();
Greenhouse secondGreenhouse = buildSecondGreenhouse();


void refreshScreen();


void setup() {
  if (ENABLE_DEBUG_OUTPUT) Serial.begin(57600);
  ns_blocker::init();
  if (ns_blocker::isBlocked()) {
    idle();
  }

  initOneWire();

  firstGreenhouse.loadSettings();
  secondGreenhouse.loadSettings();


  ns_ds18b20::init();
  ns_menu::init();
  ns_screen::init();
  ns_encoder::init();

  buildFirstGreenhouseMenu();
  buildSecondGreenhouseMenu();
  buildScreenMenu();

  if (ENABLE_DEBUG_OUTPUT) Serial.println(F("Setup successfully"));
}


void loop() {
  if (ns_blocker::isBlocked()) {
    Serial.println(F("Greenhouse is blocked"));
    idle();
  }
  // Serial.println(F("firstGreenhouse.loop()"));
  firstGreenhouse.loop();
  // Serial.println(F("secondGreenhouse.loop()"));
  secondGreenhouse.loop();
  // Serial.println(F("refreshScreen()"));
  refreshScreen();
  // Serial.println(F("ns_screen::loop()"));
  ns_screen::loop();
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
    sprintf(secondRow, "Y:%s G:%s", buf[0], buf[1]);
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
  item.value = reinterpret_cast<int8_t*>(&firstGreenhouse.openingSteps);
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

