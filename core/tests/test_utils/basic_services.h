#ifndef _DOUSI_TEST_UTILS_BASIC_SERVICES_H_
#define _DOUSI_TEST_UTILS_BASIC_SERVICES_H_

#include "../src/core/dousi.h"

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


#endif
