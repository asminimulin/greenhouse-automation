#include <global_singletons.hpp>
#include <config.hpp>

namespace {
    OneWire oneWire;
    DS2413Driver<5> ds2413Driver;
}

void initOneWire() {
    oneWire.begin(ONE_WIRE_PIN);
}

OneWire* getOneWire() {
    return &oneWire;
}

void initDS2413Driver () {}

DS2413Driver<5>* getDS2413Driver() {
    return &ds2413Driver;
}
