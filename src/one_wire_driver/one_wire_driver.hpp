#ifndef ONE_WIRE_DRIVER_HPP
#define ONE_WIRE_DRIVER_HPP


// Disable unused features
#define ONEWIRE_CRC8_TABLE 0
#define ONEWIRE_CRC16 0

#include <OneWire.h>

void initOneWire();

OneWire* getOneWire();

#endif
