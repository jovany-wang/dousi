

#include <iostream>

#include "runtime/distributed/processor/processor.h"
#include "core/common/remote_annotation.h"
#include "services.h"

using namespace dousi;
using namespace dousi::distributed;

int main() {
    std::cout << "Processor2..." << std::endl;

    constexpr auto master_address = "127.0.0.1:10001";
    Processor processor {master_address};
    auto service = processor.CreateService<TestService2>();
    service.RegisterMethod(dousi::Remote(&TestService2::IncrAndGet));

    std::this_thread::sleep_for(std::chrono::milliseconds {5 * 1000});
    auto service_handle = processor.GetService("TestService1");
    auto future = service_handle.Call(dousi::Remote(&TestService1::Ping));
    std::cout << "The result of  TestService1::Ping is " << *future.Get() << std::endl;
    return 0;
}
