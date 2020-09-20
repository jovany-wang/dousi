
#include <iostream>

#include "services.h"
#include "core/dousi.h"
#include "runtime/direct/rpc_client.h"

int main() {
    using namespace dousi;

    dousi::RpcClient rpc_client {"127.0.0.1:10001"};

    {
        // TODO(qwang): This should refined as a general type call.
        ServiceHandle adder1 = rpc_client.GetService("Adder");
        DousiFuture<int> sum_future1 = adder1.Call(Remote(&Adder::add), 2, 3);
        const auto sum = *sum_future1.Get();
        DOUSI_LOG(INFO) << "sum_future1 = " << sum;

        int sub = *adder1.Call(Remote(&Adder::sub), 100, 25).Get();
        DOUSI_LOG(INFO) << "sub_future1 = " << sub;
    }
    {
        ServiceHandle echoer = rpc_client.GetService("Echoer");
        auto echoed_str = echoer.Call(Remote(&Echoer::echo), "hello world!").Get();
        DOUSI_LOG(INFO) << "echoed_str = " << *echoed_str;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100 * 1000));
}
