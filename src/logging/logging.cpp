#include "logging.hpp"

namespace logging {

namespace private_ {
Stream* stream_;
LoggingLevel loggingLevel_;
}  // namespace private_

void init(LoggingLevel loggingLevel, Stream* stream) {
  private_::stream_ = stream;
  private_::loggingLevel_ = loggingLevel;
}

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

}  // namespace logging