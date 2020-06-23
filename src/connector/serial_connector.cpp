#include "serial_connector.hpp"

#include <Arduino.h>

namespace serial_connector {

namespace {
enum Command {
  COMMAND_PING = '0',
};
}

SerialConnector::SerialConnector(Stream* stream, Handler handler,
                                 uint8_t pingEveryNTimes, uint32_t readTimeout)
    : readTimeout_(readTimeout),
      stream_(stream),
      handler_(handler),
      pingEveryNTimes_(pingEveryNTimes),
      downTimer_(0) {}

bool SerialConnector::begin() {
  isConnected_ = connect();
  downTimer_ = pingEveryNTimes_;
  return isConnected_;
}

bool SerialConnector::connect() {
  while (Serial.available()) Serial.read();
  return ensureConnected();
}

void SerialConnector::loop() {
  if (!isConnected_) return;
  // downTimer_ -= 1;
  // if (downTimer_ == 0) {
  //   isConnected_ = ensureConnected();
  //   downTimer_ = pingEveryNTimes_;
  //   return;
  // }
  if (stream_->available() && nullptr != handler_) {
    handler_(stream_);
  }
}

bool SerialConnector::isConnected() const { return isConnected_; }

bool SerialConnector::ensureConnected() {
  stream_->write(COMMAND_PING);
  stream_->flush();
  return readSync(readTimeout_) == COMMAND_PING;
}

uint8_t SerialConnector::readSync(uint32_t timeout) {
  uint32_t stop = (timeout == 0) ? ~0u : millis() + timeout;
  if (timeout == 0)
    stop = ~0u;
  else
    stop = millis() + timeout;
  while (!stream_->available() && millis() < stop) {
  }
  if (!stream_->available()) {
    return EOS;
  }
  return static_cast<uint8_t>(stream_->read());
}

}  // namespace serial_connector
