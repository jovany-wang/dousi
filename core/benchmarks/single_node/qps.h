#pragma once

#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>

/// TODO(qwang): Refine this by thread local.
class Qps {
public:
    void increase() {
        counter_.fetch_add(1, std::memory_order_release);
    }

    Qps() : counter_(0) {
        thd_ = std::thread([this] {
            while (!stop_) {
                const auto counter = counter_.load(std::memory_order_acquire);
                std::cout << "Current qps is " << (counter - last_counter_) << ", sum = " << counter << ".\n";
                last_counter_ = counter;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                if (counter > 10000 * 10000) {
                    // Unlikely code path.
                    counter_.store(0, std::memory_order_acquire);
                }
            }
        });
    }

    ~Qps() {
        stop_ = true;
        thd_.join();
    }

private:
    bool stop_ = false;
    std::thread thd_;
    std::atomic<uint32_t> counter_;
    uint32_t last_counter_ = 0;
};
