#include "benchmark_service.h"

#include "core/common/remote_annotation.h"
#include "core/executor/dousi_service.h"
#include "runtime/direct/rpc_client.h"

//#include <core/submitter/dousi_future.h>
#include <chrono>
#include <thread>

int main(int argc, char *argv[]) {
    int thread_num = 12;
    int call_interval_ms = 2;

    std::vector<std::thread> threads;
    for (int i = 0; i < thread_num; ++i) {
        std::thread th([call_interval_ms]() {
            dousi::RpcClient rpc_client {"127.0.0.1:10002"};
            auto bm_service = rpc_client.GetService("BenchmarkService");
            int i = 0;
            while(true) {
                bm_service.Call(dousi::Remote(&BenchmarkService::Echo), "hello world.");
                std::this_thread::sleep_for(std::chrono::microseconds(call_interval_ms));
            }
            std::cout << "Finished th id = " << std::this_thread::get_id() << std::endl;
            int a;
            std::cin >> a;
        });

        threads.emplace_back(std::move(th));
    }

    for (auto &th : threads) {
        th.join();
    }

    return 0;
}
