#include "benchmark_service.h"

#include "core/common/remote_annotation.h"
#include "core/executor/dousi_service.h"
#include "runtime/direct/rpc_client.h"

#include <chrono>
#include <thread>

int main(int argc, char *argv[]) {
    dousi::RpcClient rpc_client {"127.0.0.1:10002"};
    auto bm_service = rpc_client.GetService("BenchmarkService");

    while(true) {
        bm_service.Call(dousi::Remote(&BenchmarkService::Echo), "hello world.");
        std::this_thread::sleep_for(std::chrono::microseconds(2));
    }

    return 0;
}
