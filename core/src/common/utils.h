#ifndef _DOUSI_COMMON_UTILS_H_
#define _DOUSI_COMMON_UTILS_H_

#include <functional>
#include <thread>

namespace dousi::common {

class Utils final {
public:
    static void AsyncTimeoutMsOnNewThread(uint64_t timeout_ms, const std::function<void()> &timeout_callback) {
        std::thread th([timeout_ms, timeout_callback]() {
            std::this_thread::sleep_for(std::chrono::milliseconds {timeout_ms});
            timeout_callback();
        });
        th.detach();
    }
};

}

#endif
