#include "benchmark_service.h"

#include "core/common/remote_annotation.h"
#include "core/executor/dousi_service.h"
#include "runtime/direct/rpc_server.h"

int main(int argc, char *argv[]) {
    dousi::RpcServer rpc_server {"127.0.0.1:10002"};
    auto bm_service = rpc_server.CreateService<BenchmarkService>();
    bm_service.RegisterMethod(dousi::Remote(&BenchmarkService::Echo));
    rpc_server.Loop();

    return 0;
}
