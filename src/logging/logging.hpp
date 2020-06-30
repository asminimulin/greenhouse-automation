// #ifndef AUTOMATION_LOGGING_LOGGING2_HPP
// #define AUTOMATION_LOGGING_LOGGING2_HPP

#pragma once

#include <Arduino.h>

namespace logging {

enum LoggingLevel : uint8_t {
  ALL,
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  NOTHING,
};

namespace private_ {
extern Stream* stream_;
extern LoggingLevel loggingLevel_;
}  // namespace private_

void init(LoggingLevel loggingLevel, Stream* stream);

class LoggingObject {
  friend LoggingObject debug();
  friend LoggingObject info();
  friend LoggingObject error();
  friend LoggingObject warning();

 public:
  template <typename T>
  LoggingObject& operator<<(const T& value) {
    if (activated_) {
      private_::stream_->print(' ');
      private_::stream_->print(value);
    }
    return *this;
  }

  ~LoggingObject() {
    if (activated_) private_::stream_->println();
  }

 private:
  bool activated_;

  LoggingObject(const __FlashStringHelper* loggingType, bool activated = true)
      : activated_(activated) {
    if (activated) private_::stream_->print(loggingType);
  }
};

LoggingObject debug();
LoggingObject info();
LoggingObject error();
LoggingObject warning();

}  // namespace logging

// #endif