#ifndef _DOUSI_COMMON_STD_LOCKED_QUEUE_H_
#define _DOUSI_COMMON_STD_LOCKED_QUEUE_H_

#include <mutex>
#include <condition_variable>
#include <queue>

namespace dousi {

template <typename T>
class StdLockedQueue {

public:
    void Push(T o) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.push(std::move(o));
        cond_var_.notify_one();
    }

    void WaitAndPop(T *result) {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_var_.wait(lock, [this]() { return !data_.empty(); });
        *result = data_.front();
        data_.pop();
    }

    bool IsEmpty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.empty();
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable cond_var_;
    std::queue<T> data_;
};

}

#endif
