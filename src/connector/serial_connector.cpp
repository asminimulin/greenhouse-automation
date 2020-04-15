#include "serial_connector.hpp"

namespace serial_connector {
    
SerialConnector::SerialConnector(Stream* stream, Handler handler):
        stream_(stream),
        handler_(handler) {}

void SerialConnector::loop() {
    if (stream_->available()) {
        handler_(stream_);
    }
}

}
