#ifndef _DOUSI_COMMON_BOOST_LOCK_FREE_QUEUE_H_
#define _DOUSI_COMMON_BOOST_LOCK_FREE_QUEUE_H_

#include <boost/lockfree/queue.hpp>
#include <thread>

namespace dousi {

template <typename T>
class BoostLockFreeQueue {
public:

    BoostLockFreeQueue() :data_(1024 * 1024) {}

    void Push(T o) {
        data_.push(o);
    }

    void WaitAndPop(T *result) {
        while (true) {
            bool ok = data_.pop(*result);
            if (ok) {
                return;
            } else {
                std::this_thread::sleep_for(std::chrono::microseconds {500});
            }
        }
    }

    [[nodiscard]] virtual uint32_t GetSize() const {
        return 0;
    }

    [[nodiscard]] virtual bool IsEmpty() const {
        return data_.empty();
    }

private:
    boost::lockfree::queue<T> data_;
};

}

#endif
