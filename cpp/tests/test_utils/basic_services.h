#ifndef _DOUSI_TEST_UTILS_BASIC_SERVICES_H_
#define _DOUSI_TEST_UTILS_BASIC_SERVICES_H_

#include <string>

#include "../src/core/dousi.h"
#include "core/executor/abstract_service.h"

class Echoer : public dousi::AbstractService {
public:
    std::string echo(const std::string &str) {
        return str;
    }
};


class Calculator : public dousi::AbstractService {
public:
    int add(int x, int y) {
        return x + y;
    }

    int sub(int x, int y) {
        return x - y;
    }
};

class VoidReturnService : public dousi::AbstractService {
public:
    void Perform(const std::string &str) {
        data_ = str;
    }

    std::string Get() {
        return data_;
    }

private:
    std::string data_ = "123";
};

#endif
