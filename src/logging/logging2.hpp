#pragma once

#include <Arduino.h>

namespace logging2 {

enum LoggingLevel : uint8_t {
  ALL,
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  NOTHING,
};

namespace private_ {
Stream* stream_;
LoggingLevel loggingLevel_;
}  // namespace private_

void init(LoggingLevel loggingLevel, Stream* stream) {
  private_::stream_ = stream;
  private_::loggingLevel_ = loggingLevel;
}

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

LoggingObject debug() {
  return LoggingObject(F("Debug:"),
                       private_::loggingLevel_ <= LoggingLevel::DEBUG);
}

LoggingObject info() {
  return LoggingObject(F("Info:"),
                       private_::loggingLevel_ <= LoggingLevel::INFO);
}

LoggingObject error() {
  return LoggingObject(F("Error:"),
                       private_::loggingLevel_ <= LoggingLevel::ERROR);
}

LoggingObject warning() {
  return LoggingObject(F("Warning:"),
                       private_::loggingLevel_ <= LoggingLevel::WARNING);
}
}  // namespace logging2
