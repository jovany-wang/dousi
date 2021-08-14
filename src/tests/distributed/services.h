#ifndef _TEST_DISTRIBUTED_SERVICES_H_
#define _TEST_DISTRIBUTED_SERVICES_H_

#include "core/executor/dousi_service.h"

class TestService1 : public dousi::AbstractService {
public:
    std::string Ping() {
        return "Pong";
    }
};

class TestService2 : public dousi::AbstractService {
public:
    int IncrAndGet(int delta) {
        return curr_.fetch_add(delta);
    }

private:
    std::atomic<int> curr_ = {0};
};

#endif
