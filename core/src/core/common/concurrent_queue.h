#ifndef _DOUSI_COMMON_CONCURRENT_QUEUE_H_
#define _DOUSI_COMMON_CONCURRENT_QUEUE_H_

#include <queue>

namespace dousi {

template <typename T>
class ConcurrentQueueInterface {

public:
    virtual void Push(T o) = 0;

    virtual void WaitAndPop(T *result) = 0;

    virtual uint32_t GetSize() const = 0;

    virtual bool IsEmpty() const = 0;

};

}

#endif
