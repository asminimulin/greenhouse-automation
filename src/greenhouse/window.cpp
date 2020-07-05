#include "window.hpp"

#include "ds2413_driver.hpp"
#include "global_singletons.hpp"
#include "logging/logging.hpp"

Window::Window(uint32_t fullTime, const DeviceAddress address) noexcept
    : device_(address), balance_(0), fullTime_(fullTime) {
  isValid_ = reinterpret_cast<const uint64_t*>(address) != 0;
  openingRoutine_.w = this;
  closingRoutine_.w = this;
  stoppingRoutine_.w = this;
}

void Window::setAddress(uint8_t* address) { device_.setAddress(address); }

bool Window::begin() {
  balance_ = fullTime_;
  stepClose(fullTime_);
  return true;
}

void Window::stepOpen(int32_t time) {
  if (!isValid_) {
    logging::error() << F("Window is invalid");
    return;
  }
  // In practise we have problem with non 100% reliable 1-Wire connection
  // So we cannot rely on balance_ at all. So right now we only count balance
  // but we do not try to increase relay life skipping useless moves
  // if (balance_ + time <= fullTime_ + timeInfelicity)
  if (true) {
    balance_ += time;
    {
      ds2413_driver::Task task(&openingRoutine_, time, &stoppingRoutine_);
      task.setName(F("Opening task"));
      if (!getDS2413Driver()->hasTask(task)) {
        getDS2413Driver()->createTask(task);
      } else {
        logging::info() << F(
            "No need to add opening task, because it is already exists");
      }
    }  // new
    balance_ = constrain(balance_, time, fullTime_ + timeInfelicity);
  } else {
    logging::info() << F("No need to open: balance is maximum");
  }
}

void Window::stepClose(int32_t time) {
  if (!isValid_) {
    logging::error() << F("Window is invalid");
    return;
  }
  // In practise we have problem with non 100% reliable 1-Wire connection
  // So we cannot rely on balance_ at all. So right now we only count balance
  // but we do not try to increase relay life skipping useless moves
  // if (balance_ - time >= -timeInfelicity) {
  if (true) {
    balance_ -= time;
    {
      ds2413_driver::Task task(&closingRoutine_, time,
                               &stoppingRoutine_);  // new
      task.setName(F("Closing task"));
      if (!getDS2413Driver()->hasTask(task)) {
        getDS2413Driver()->createTask(task);
      } else {
        logging::info() << F(
            "No need to add closing task, because it is already exists");
      }
    }
    balance_ = constrain(balance_, -timeInfelicity, fullTime_ - time);
  } else {
    logging::info() << F("No need to close: balance is minimum");
  }
}
