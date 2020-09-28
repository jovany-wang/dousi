

#include <iostream>

#include "runtime/distributed/processor/processor.h"
#include "core/common/remote_annotation.h"

#include "services.h"

using namespace dousi;
using namespace dousi::distributed;

int main() {
    std::cout << "Processor1..." << std::endl;

    constexpr auto master_address = "127.0.0.1:10001";
    Processor processor {master_address};
    auto service = processor.CreateService<TestService1>();
    service.RegisterMethod(dousi::Remote(&TestService1::Ping));

    std::this_thread::sleep_for(std::chrono::milliseconds {5 * 1000});
    auto service_handle = processor.GetService("TestService2");
    {
        auto future = service_handle.Call(dousi::Remote(&TestService2::IncrAndGet), 10);
        std::cout << "Result of TestService2::IncrAndGet is " << *future.Get() << std::endl;
    }
    {
        auto future = service_handle.Call(dousi::Remote(&TestService2::IncrAndGet), 10);
        std::cout << "Result of TestService2::IncrAndGet is " << *future.Get() << std::endl;
    }
    return 0;
}
