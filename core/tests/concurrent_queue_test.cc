#include "gtest/gtest.h"

#include <functional>
#include "common/utils.h"
#include "core/common/boost_lock_free_queue.h"
#include <thread>


TEST(ConcurrentQueue, TestBoostLockFreeQueue) {
    dousi::BoostLockFreeQueue<int> queue;
    std::thread th1([&queue]() {
        for (int i = 0; i < 10000; ++i) {
            queue.Push(i);
        }
    });

    std::thread th2([&queue]() {
        for (int i = 0; i < 10000; ++i) {
            queue.Push(i);
        }
    });

    std::atomic<bool> is_timeout = {false};
    dousi::common::Utils::AsyncTimeoutMsOnNewThread(2 * 1000, [&is_timeout]() {
        is_timeout.store(true);
    });
    int count = 0;
    int result;
    while(count < 20000 && !is_timeout) {
        queue.WaitAndPop(&result);
        count += 1;
    }
    ASSERT_EQ(count, 20000);
    th1.join();
    th2.join();
    ASSERT_EQ(queue.GetSize(), 0);

    // Test GetSize
    for (int i = 0; i < 10000; ++i) {
        queue.Push(i);
    }

//    ASSERT_EQ(queue.GetSize(), 10000);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
