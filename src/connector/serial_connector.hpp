#ifndef SERIAL_CONNECTOR_HPP
#define SERIAL_CONNECTOR_HPP

#include <Stream.h>

namespace serial_connector {

using Handler = void (*)(Stream* stream);

inline void echoer(Stream* stream) {
  if (stream->available()) {
    stream->write(stream->read());
  }
}

class SerialConnector {
 public:
  SerialConnector(Stream* stream, Handler cb = echoer,
                  uint8_t pingEveryNTimes = 20, uint32_t readTimeout = 500);
  bool begin();
  void loop();
  bool isConnected() const;

 private:
  bool connect();
  bool ensureConnected();
  uint8_t readSync(uint32_t timeout = 0);

 private:
  uint32_t readTimeout_;
  Stream* stream_;
  Handler handler_;
  uint8_t pingEveryNTimes_;
  uint8_t downTimer_;
  bool isConnected_;

 private:
  enum SpecialCharacters : uint8_t { EOS = 255 };
};

}  // namespace serial_connector

#endif