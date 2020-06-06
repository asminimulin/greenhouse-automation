#ifndef DS2413_DRIVER_HPP
#define DS2413_DRIVER_HPP

#include <stdint-gcc.h>
#include <assert.h>
#include <logging/logging.hpp>
#include "idle.hpp"

/* ****************************************************************************
 *
 *  It is async implementation of ds2413 driver, which provide sync
 *  execution of timed commands one by one
 *
 *****************************************************************************/

namespace ds2413_driver
{

class Task
{
public:
    struct routine_t {
        virtual void operator()() {};
    };


public:
    Task() = default;
    explicit Task(routine_t* beforeRoutine,  uint32_t delay = 0, routine_t* afterRoutine = nullptr) noexcept;
    explicit Task(Task&& task) = default;
    explicit Task(const Task& task) = default;
    Task& operator=(Task&& task) = default;
    Task& operator=(const Task& task) = default;
    bool operator==(const Task& other) const noexcept;
    inline bool isComplete() const noexcept { return state_ == STATE_COMPLETE; }
    void loop();
    inline void setName(const __FlashStringHelper* name) noexcept { name_ = name; }


private:
    enum state_t: uint8_t
    {
        STATE_NOT_RUNNING,
        STATE_DELAY,
        STATE_COMPLETE,
    };


private:
    uint32_t delay_;
    uint32_t delayCompleteTime_;
    routine_t* beforeRoutine_;
    routine_t* afterRoutine_;
    const __FlashStringHelper* name_;
    state_t state_;
};

}


template<int8_t MAX_TASKS_COUNT>
class DS2413Driver
{
    using Task = ds2413_driver::Task;
    using pos_t = int8_t;


public:
    DS2413Driver() noexcept
    {
        head_ = 0;
        tail_ = 0;
        allowDuplicates_ = false;
        assert(MAX_TASKS_COUNT > 0);
    }

    // Check if literally the same tasks is queued
    bool hasTask(const Task& task)
    {
        if (isEmpty()) return false;
        pos_t i = head_;
        for (pos_t i = head_; i != tail_; i = (i + 1) % MAX_TASKS_COUNT) {
            if (task == queue_[i]) return true;
        }
        return false;
    }

    // Entry point for running scheduler
    void loop()
    {
        if (isEmpty()) return;
        Task& task = this->frontTask();
        task.loop();
        if (task.isComplete()) {
            popTask();
        }
    }

    // Duplication configuration
    inline bool getAllowDuplicatedTasks() const noexcept
    {
        return allowDuplicates_;
    }

    void setAllowDuplicatedTasks(bool allowDuplicates) noexcept
    {
        allowDuplicates_ = allowDuplicates;
    }

    // Add task to queue
    void createTask(const Task& task)
    {
        if (isFull()) {
            logging::warning(F("Queue is full. Cannot insert new task"));
            return;
        }
        if (hasTask(task) && !allowDuplicates_) {
            logging::warning(F("Duplicated task."));
            return;
        }
        appendTask(task);
    }

    // Check if queue if full
    inline bool isFull() const noexcept
    {
        return (tail_ + 1) % MAX_TASKS_COUNT == head_;
    }

    // Check if queue is empty
    inline bool isEmpty() const noexcept { return head_ == tail_; }


private:
    void appendTask(const Task& task) noexcept
    {
        // Here we know for sure, that queue is not full
        queue_[tail_] = task;
        tail_ = (tail_ + 1) % MAX_TASKS_COUNT;
        logging::info(F("Task appended"));
    }

    void popTask() noexcept {
        // Here we know for sure that queue is not empty
        head_ = (head_ + 1) % MAX_TASKS_COUNT;
        logging::info(F("Task removed"));
    }

    Task& frontTask()
    {
        if (isEmpty()) {
            logging::error(F("Queue is empty. But trying to access front task."));
            idle();
        }
        return queue_[head_];
    }

private:
    Task queue_[MAX_TASKS_COUNT];
    pos_t head_;
    pos_t tail_;
    bool allowDuplicates_;
};

#endif  // DS2413_DRIVER_HPP
