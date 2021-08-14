#ifndef _DOUSI_BENCHMARKS_SINGLE_NODE_
#define _DOUSI_BENCHMARKS_SINGLE_NODE_

#include "qps.h"
#include "core/executor/abstract_service.h"
#include <string>

class BenchmarkService : public dousi::AbstractService {
public:

    std::string Echo(const std::string &str) {
        qps_.increase();
        return str;
    }

    std::string SlowlyEcho(const std::string &str) {
        qps_.increase();
        std::this_thread::sleep_for(std::chrono::milliseconds {10});
        return str;
    }

private:
    Qps qps_;
};

#endif
