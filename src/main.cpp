#include <Arduino.h>

#include "config.hpp"
#include "connector/serial_connector.hpp"
#include "display/display.hpp"
#include "display/green_window_screen.hpp"
#include "display/menu_screen.hpp"
#include "display/outside_screen.hpp"
#include "display/yellow_window_screen.hpp"
#include "encoder/encoder.hpp"
#include "global_singletons.hpp"
#include "greenhouse/ds2413_driver.hpp"
#include "greenhouse/greenhouse.hpp"
#include "idle.hpp"
#include "logging/logging.hpp"
#include "logging/logging2.hpp"
#include "menu/menu.hpp"
#include "menu/menu_validators.hpp"
#include "screen_holder.hpp"

void buildGreenhouseMenu();
void buildDisplayMenu();
void buildSummerModeMenu();
void buildResetMenu();
Greenhouse buildGreenhouse();

Greenhouse greenhouse = buildGreenhouse();

/* Screens */
static constexpr uint8_t totalScreensCount = 3;
YellowWindowScreen yellowWindowScreen(&greenhouse);
GreenWindowScreen greenWindowScreen(&greenhouse);
OutsideScreen outsideScreen(&greenhouse);
MenuScreen menuScreen;
ScreenHolder<totalScreensCount> screenHolder;

Display display(LCD_ADDRESS);

void espHandle(Stream* stream);
serial_connector::SerialConnector espConnector(&Serial, espHandle);

void skipEspTrashWriting();

void notifySystemBlocked();

void setup() {
  Serial.begin(57600);

  skipEspTrashWriting();

  if (ENABLE_DEBUG_OUTPUT) {
    logging::setup(logging::DEBUG, &Serial);
    logging2::init(logging2::DEBUG, &Serial);
  } else {
    logging::setup(logging::NOTHING, &Serial);
    logging2::init(logging2::NOTHING, &Serial);
  }
  /*
   * Initializing 1-Wire bus
   */
  logging::info(F("Initializing 1-Wire bus..."));
  initOneWire();
  logging::info(F("1-Wire bus is ready."));

  logging::info(F("Initializing greenhouse..."));
  greenhouse.loadSettings();
  logging::info(F("Greenhouse is ready."));

  logging::info(F("Initializing ds18b20 protocol implementation..."));
  ns_ds18b20::init();
  logging::info(F("ds18b20 protocol is ready to use."));

  logging::info(F("Initializing menu..."));
  ns_menu::init();
  logging::info(F("menu is ready"));

  logging::info(F("Initializing display..."));
  screenHolder.registerScreen(&yellowWindowScreen);
  screenHolder.registerScreen(&greenWindowScreen);
  screenHolder.registerScreen(&outsideScreen);
  display.begin();
  display.setScreen(screenHolder.getActiveScreen());
  logging::info(F("display is ready"));

  logging::info(F("Initializing encoder..."));
  ns_encoder::init();
  logging::info(F("encoder is ready"));

  logging::info(F("Building menu"));
  buildDisplayMenu();
  buildSummerModeMenu();
  buildGreenhouseMenu();
  buildResetMenu();
  logging::info(F("Menu is ready"));

  /*
   * Estabilishing connection with esp8266 module
   */
  logging::info(F("Estabilishing connection with esp8266 module"));
  if (espConnector.begin()) {
    logging::info(F("Connection with esp8266 successfully estabilished"));
  } else {
    logging::error(F("Failed to estabilish connection with esp8266"));
  }

  logging::info(F("System is ready."));

  logging::info(F("Starting greenhouse..."));
  if (greenhouse.begin()) {
    logging::info(F("Greenhouse successfully started."));
  } else {
    logging::error(F("Failed to start greenhouse."));
    idle();
  }
}

void loop() {
  greenhouse.loop();
  espConnector.loop();
  getDS2413Driver()->loop();
  display.loop();
  ns_ds18b20::refreshTemperatures();
}

Greenhouse buildGreenhouse() {
  GreenhouseConfig config;
  memcpy(config.yellowSensorAddress, YELLOW_SENSOR_ADDRESS, 8);
  memcpy(config.greenSensorAddress, GREEN_SENSOR_ADDRESS, 8);
  memcpy(config.outsideSensorAddress, OUTSIDE_SENSOR_ADDRESS, 8);
  memcpy(config.yellowMotorAddress, YELLOW_MOTOR_ADDRESS, 8);
  memcpy(config.greenMotorAddress, GREEN_MOTOR_ADDRESS, 8);
  memcpy(config.ventAddress, VENT_ADDRESS, 8);
  config.openingTime = OPENING_TIME;
  config.temperatureInnercyDelay = TEMPERATURE_INNERCY_DELAY;
  return Greenhouse(config, EEPROM_PROPERTIES_ADDRESS);
}

void ns_encoder::increment() {
  if (ns_menu::isEnabled()) {
    ns_menu::handleIncrement();
  } else {
    screenHolder.next();
    display.setScreen(screenHolder.getActiveScreen());
  }
  display.resetInterruptTimer();
}

void ns_encoder::decrement() {
  if (ns_menu::isEnabled()) {
    ns_menu::handleDecrement();
  } else {
    screenHolder.previous();
    display.setScreen(screenHolder.getActiveScreen());
  }
  display.resetInterruptTimer();
}

void ns_encoder::incrementPress() {
  ns_menu::handleIncrementPress();
  if (ns_menu::isEnabled()) {
    display.setScreen(&menuScreen);
  } else {
    display.setScreen(screenHolder.getActiveScreen());
  }
  display.resetInterruptTimer();
}

void ns_encoder::decrementPress() {
  ns_menu::handleDecrementPress();
  if (ns_menu::isEnabled()) {
    display.setScreen(&menuScreen);
  } else {
    display.setScreen(screenHolder.getActiveScreen());
  }
  display.resetInterruptTimer();
}

void ventModeRepresenter(int8_t value, char* buffer) {
  if (value == Greenhouse::VentMode::VENT_DISABLE) {
    strcpy_P(buffer, reinterpret_cast<PGM_P>(F("OFF")));
  } else if (value == Greenhouse::VENT_ENABLE) {
    strcpy_P(buffer, reinterpret_cast<PGM_P>(F("ON")));
  } else if (value == Greenhouse::VENT_AUTO) {
    strcpy_P(buffer, reinterpret_cast<PGM_P>(F("AUTO")));
  } else {
    logging2::error() << F("ventModeRepresnter got invalid value.");
    idle();
  }
}

bool ventModeValidator(int8_t value) { return value >= 0 && value < 3; }

void buildGreenhouseMenu() {
  MenuItem item;
  auto rootId = ns_menu::getRoot();

  item.name = F("Opening Temp");
  item.parent = rootId;
  item.value = &greenhouse.openingTemperature;
  item.validator = validateOpeningTemperature;
  item.isLeaf = true;
  ns_menu::addItem(item);

  item.name = F("Closing Temp");
  item.parent = rootId;
  item.value = &greenhouse.closingTemperature;
  item.isLeaf = true;
  item.validator = validateClosingTemperature;
  ns_menu::addItem(item);

  item.name = F("Opening steps");
  item.parent = rootId;
  item.value = reinterpret_cast<int8_t*>(&greenhouse.openingSteps);
  item.isLeaf = true;
  item.validator = validateOpeningSteps;
  ns_menu::addItem(item);

  item.name = F("Vent mode");
  item.parent = rootId;
  item.value = reinterpret_cast<int8_t*>(&greenhouse.ventMode_);
  item.isLeaf = true;
  item.represent = ventModeRepresenter;
  item.hasCustomRepresenation = true;
  item.validator = ventModeValidator;
  ns_menu::addItem(item);
}

void buildDisplayMenu() {
  MenuItem item;
  item.parent = ns_menu::getRoot();
  item.name = F("Screen light");
  item.isLeaf = true;
  item.value = reinterpret_cast<int8_t*>(&display.screenLightingSetting_);
  item.hasCustomRepresenation = true;
  item.represent = ns_display::screenLightSettingRepresenter;
  item.validator = ns_display::screenLightSettingValidator;
  ns_menu::addItem(item);
}

void repsentSummerMode(int8_t value, char* buffer) {
  if ((value & 1) == 0) {
    strcpy_P(buffer, reinterpret_cast<PGM_P>(F("OFF")));
  } else {
    strcpy_P(buffer, reinterpret_cast<PGM_P>(F("ON")));
  }
}

void buildSummerModeMenu() {
  MenuItem item;
  item.parent = ns_menu::getRoot();
  item.name = F("Summer mode");
  item.isLeaf = true;
  item.value = reinterpret_cast<int8_t*>(&(greenhouse.summerMode));
  item.hasCustomRepresenation = true;
  item.represent = repsentSummerMode;
  ns_menu::addItem(item);
}

enum EspCommandInterfaces : uint8_t {
  COMMAND_GET_MEASURES = '1',
  COMMAND_GET_SETTINGS = '2',
};
enum StatusCode : uint8_t {
  OK = 0,
  ERROR = 1,
};
void espHandle(Stream* stream) {
  auto command = stream->read();  // we for sure know that stream is not empty
                                  // (see implementation of SerialConnector)
  if (command == COMMAND_GET_MEASURES) {
    auto yellowTemperature =
        static_cast<uint8_t>(greenhouse.getYellowTemperature());
    auto greenTemperature =
        static_cast<uint8_t>(greenhouse.getGreenTemperature());
    auto outsideTemperature =
        static_cast<uint8_t>(greenhouse.getOutsideTemperature());
    stream->write(OK);
    uint8_t messageSize = 10;  // measures size
    stream->write(messageSize);
    stream->write(outsideTemperature);

    stream->write(yellowTemperature);
    stream->write(greenTemperature);
    stream->write(uint8_t(greenhouse.getVentStatus()));
    stream->write(greenhouse.getYellowPerCent());  // yellow window per cent
    stream->write(greenhouse.getGreenPerCent());   // green window per cent
    stream->write(uint8_t(0));                     // blue humidity
    stream->write(uint8_t(false));                 // blue watering status
    stream->write(uint8_t(0));                     // red humidity
    stream->write(uint8_t(false));                 // red watering status

  } else if (command == COMMAND_GET_SETTINGS) {
    stream->write(OK);
    uint8_t messageSize = 3;  // settings size
    stream->write(uint8_t(messageSize));

    stream->write(uint8_t(greenhouse.getOpeningTemperature()));
    stream->write(uint8_t(greenhouse.getClosingTemperature()));
    stream->write(greenhouse.getStepsCount());
  } else {
    /* Unsupported command -> do nothing */
    logging::error(F("Unsupported command"));
  }
}

void skipEspTrashWriting() {
  delay(2000);
  while (Serial.available()) {
    Serial.read();
  }
}

static class : public Screen {
  inline void renderFirst() final {
    char* row = getFirstBuffer();
    strcpy_P(row, reinterpret_cast<PGM_P>(F("   Stop test    ")));
  }

  inline void renderSecond() final {
    char* row = getSecondBuffer();
    strcpy_P(row, reinterpret_cast<PGM_P>(F("   Time is up   ")));
  }
} blockedScreen;

void notifySystemBlocked() {
  logging2::info() << F("Testing time finished. System stopped");
  display.setScreen(&blockedScreen);
  display.loop();
}
