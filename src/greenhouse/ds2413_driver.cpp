#include "ds2413_driver.hpp"

#include "idle.hpp"

namespace ds2413_driver {

Task::Task(routine_t* beforeRoutine, uint32_t delay, routine_t* afterRoutine)
    : delay_(delay),
      beforeRoutine_(beforeRoutine),
      afterRoutine_(afterRoutine) {
  state_ = STATE_NOT_RUNNING;
  name_ = nullptr;
}

bool Task::operator==(const Task& other) const noexcept {
  return this->beforeRoutine_ == other.beforeRoutine_ &&
         this->afterRoutine_ == other.afterRoutine_ &&
         this->delay_ == other.delay_;
}

void Task::loop() {
  if (state_ == STATE_NOT_RUNNING) {
    if (nullptr != beforeRoutine_) {
      (*beforeRoutine_)();
    }
    state_ = STATE_DELAY;
    delayCompleteTime_ = millis() + delay_;

  } else if (state_ == STATE_DELAY) {
    if (millis() > delayCompleteTime_) {
      if (nullptr != afterRoutine_) {
        (*afterRoutine_)();
      }
      state_ = STATE_COMPLETE;
    }

  } else if (state_ == STATE_COMPLETE) {
    logging::warning() << F(
        "DS2413Driver loops over the completed task. Probably unexpected "
        "idle.");

  } else {
    logging::error() << F("DS2413Driver is broken. Task has invalid state.");
    idle();
  }
}

}  // namespace ds2413_driver