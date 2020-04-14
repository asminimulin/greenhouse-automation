#include <Arduino.h>


#include "encoder.hpp"
#include "config.hpp"


namespace {

void encoderHandler() {
    if (digitalRead(ENCODER_INTERRUPT_PIN)) return;
    digitalWrite(13, !digitalRead(13));
    // Serial.println(F("Interrupt"));
    bool direction = digitalRead(ENCODER_DIRECTION_PIN);
    bool press = digitalRead(ENCODER_PRESS_PIN);
    // Serial.print(F("Direction: "));
    // Serial.println(direction);
    // Serial.print(F("Press: "));
    // Serial.println(press);
    if (direction && press) {
        ns_encoder::incrementPress();
    } else if (direction) {
        ns_encoder::increment();
    } else if (press) {
        ns_encoder::decrementPress();
    } else {
        ns_encoder::decrement();
    }
}

}


namespace ns_encoder {


void init() {
    uint8_t pin = ENCODER_INTERRUPT_PIN;
    attachInterrupt(digitalPinToInterrupt(pin), encoderHandler, CHANGE);
    pinMode(13, OUTPUT);
}

}