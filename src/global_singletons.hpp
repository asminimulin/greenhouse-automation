#ifndef ONE_WIRE_DRIVER_HPP
#define ONE_WIRE_DRIVER_HPP

// Disable unused features
#define ONEWIRE_CRC8_TABLE 0
#define ONEWIRE_CRC16 0

#include <OneWire.h>

#include <greenhouse/ds2413_driver.hpp>

void initOneWire();
OneWire* getOneWire();

void initDS2413Driver();
DS2413Driver<9>* getDS2413Driver();

#endif
