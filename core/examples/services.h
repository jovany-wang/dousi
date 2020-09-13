#ifndef _EXAMPLES_SERVICES_H_
#define _EXAMPLES_SERVICES_H_

#include "core/executor/abstract_service.h"

#include <string>

class Adder : public dousi::AbstractService {
public:
    int add(int x, int y) {
        return x + y;
    }

    int sub(int x, int y) {
        return x - y;
    }
};

class Echoer : public dousi::AbstractService {
public:
    std::string echo(const std::string &str) {
        return str;
    }
};

#endif
