
#include <iostream>

#include "runtime/distributed/master/master_server.h"

using namespace dousi;
using namespace dousi::master;

int main() {
    constexpr auto master_address = "127.0.0.1:10001";
    MasterServer master_server {master_address};
    return 0;
}
