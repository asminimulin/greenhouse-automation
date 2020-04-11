#include <one_wire_driver.hpp>
#include <config.hpp>

namespace {
    OneWire oneWire;
}

void initOneWire() {
    oneWire.begin(ONE_WIRE_PIN);
}

OneWire* getOneWire() {
    return &oneWire;
}
