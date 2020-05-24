#include "window.hpp"


Window::Window(uint32_t fullTime, const DeviceAddress address) noexcept
    : device_(address)
    , balance_(0)
    , fullTime_(fullTime) {
        isValid_ = reinterpret_cast<const uint64_t*>(address) != 0;
    }


void Window::stepOpen(int32_t time) {
    if (balance_ + time <= fullTime_ + timeInfelicity) {
        balance_ += time;
        device_.setStateFor(OPENINIG, time);
        balance_ = constrain(balance_, 0, fullTime_ + timeInfelicity);
    }
}


void Window::stepClose(int32_t time) {
    if (balance_ - time >= -timeInfelicity) {
        balance_ -= time;
        device_.setStateFor(CLOSING, time);
        balance_ = constrain(balance_, -timeInfelicity, fullTime_);
    }
}
