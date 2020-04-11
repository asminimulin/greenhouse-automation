#ifndef IDLE_HPP
#define IDLE_HPP
#include <Arduino.h>

inline void idle() {
    Serial.println(F("idle()"));
    interrupts();
    while(1) {
        Serial.println(F("idle()"));
        delay(5000lu);
    }
}

#endif