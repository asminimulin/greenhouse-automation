#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <Arduino.h>


namespace logging {

enum LoggingLevel {
    ALL,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    NOTHING,
};


void setup(LoggingLevel level, Print* output);


void debug(const __FlashStringHelper* message);

template<typename T>
void debug(const T& arg) {
    Serial.println(arg);
}


void error(const __FlashStringHelper* message);

template<typename T>
void error(const T& arg) {
    Serial.print("Error: ");
    Serial.println(arg);
}


void warning(const __FlashStringHelper* message);

template<typename T>
void warning(const T& arg) {
    Serial.print("Warning: ");
    Serial.println(arg);
}

void info(const __FlashStringHelper* message);

template<typename T>
void info(const T& arg) {
    Serial.print("Info: ");
    Serial.println(arg);
}


void debug(const int& i);

}

#endif