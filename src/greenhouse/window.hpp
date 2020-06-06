#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <OneWire.h>
#include "stdint.h"
#include "ds2413.hpp"
#include "one_wire_address_progmem.hpp"
#include "ds2413_driver.hpp"


typedef uint8_t DeviceAddress[8];

class Window {
public:
    Window(uint32_t openingTime, const DeviceAddress address) noexcept;
    void stepOpen(int32_t time);
    void stepClose(int32_t time);
    inline void loop() { device_.loop(); }
    inline bool isBusy() { return device_.isBusy(); }
    inline uint8_t getPerCent() const noexcept { return uint8_t((constrain(balance_, 0, fullTime_) / float(fullTime_)) * 100); }


private:
    DS2413 device_;
    int32_t balance_;
    const int32_t fullTime_;
    static constexpr int32_t timeInfelicity = 10 * 1000;
    bool isValid_;

    enum DeviceState: uint8_t {
        OPENING = 0b01,
        CLOSING = 0b10,
        STOPPED = 0b11,
    };


    using Routine = ds2413_driver::Task::routine_t;
    struct OpeningRoutine: Routine {
        Window* w;
        void operator()() override {
            w->device_.setState(OPENING);
        };
    } openingRoutine_;
    friend class OpeningRoutine;

    struct ClosingRoutine: Routine {
        Window* w;
        void operator()() override {
            w->device_.setState(CLOSING);
        };
    } closingRoutine_;
    friend class ClosingRoutine;

    struct StoppingRoutine: Routine {
        Window* w;
        void operator()() override {
            w->device_.setState(STOPPED);
        };
    } stoppingRoutine_;
    friend class StoppedRoutine;
};

#endif