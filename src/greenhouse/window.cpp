#include "window.hpp"
#include "logging/logging.hpp"
#include "ds2413_driver.hpp"
#include "global_singletons.hpp"


Window::Window(uint32_t fullTime, const DeviceAddress address) noexcept
    : device_(address)
    , balance_(0)
    , fullTime_(fullTime)
{
    isValid_ = reinterpret_cast<const uint64_t*>(address) != 0;
    openingRoutine_.w = this;
    closingRoutine_.w = this;
    stoppingRoutine_.w = this;
}


void Window::stepOpen(int32_t time)
{
    if (!isValid_) {
        logging::warning(F("Window is invalid"));
        return;
    }
    if (balance_ + time <= fullTime_ + timeInfelicity) {
        balance_ += time;
        // device_.setStateFor(OPENINIG, time);  // old
        {
            ds2413_driver::Task task(&openingRoutine_, time, &stoppingRoutine_);
            if (!getDS2413Driver()->hasTask(task)) {
                getDS2413Driver()->createTask(task);
            } else {
                logging::info(F("No need to add opening task, because it is already exists"));
            }
        }                                        // new
        balance_ = constrain(balance_, time, fullTime_ + timeInfelicity);
    } else {
        logging::info(F("No need to open: balance is maximum"));
    }
}


void Window::stepClose(int32_t time)
{
    if (!isValid_) return;
    if (balance_ - time >= -timeInfelicity) {
        balance_ -= time;

        // device_.setStateFor(CLOSING, time);                              // old
        {
            ds2413_driver::Task task(&closingRoutine_, time, &stoppingRoutine_);  // new 
            if (!getDS2413Driver()->hasTask(task)) {
                getDS2413Driver()->createTask(task);
            } else {
                logging::info(F("No need to add closing task, because it is already exists"));
            }
        }
        balance_ = constrain(balance_, -timeInfelicity, fullTime_ - time);
    } else {
        logging::info(F("No need to close: balance is minimum"));
    }
}
