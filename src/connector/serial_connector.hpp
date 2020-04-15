#ifndef SERIAL_CONNECTOR_HPP
#define SERIAL_CONNECTOR_HPP

#include <Stream.h>


namespace serial_connector {

using Handler = void(*)(Stream* stream);


inline void echoer(Stream* stream) {
    if (stream->available()) {
        stream->write(stream->read());
    }
}


class SerialConnector {
public:
    SerialConnector(Stream* stream, Handler cb = echoer);
    void loop();

private:
    Stream* stream_;
    Handler handler_;
};

}

#endif