#ifndef _DOUSI_RUNTIME_DIRECT_RPC_SERVER_H
#define _DOUSI_RUNTIME_DIRECT_RPC_SERVER_H


#include "core/executor/executor.h"

namespace dousi {

class RpcServer {
public:
   explicit RpcServer(const std::string &listening_address) {
       dousi::DousiLog::StartDousiLog("/tmp/dousi/rpcserver.log",
                                      dousi::LOG_LEVEL::FATAL, 10, 3);
       executor_->Init(listening_address);
   }

    template<typename ServiceType>
    DousiService<ServiceType> CreateService() {
        const auto service_name = std::string(NAMEOF_TYPE(ServiceType));
        return executor_->CreateService<ServiceType>(service_name);
    }

    void Loop() {
       executor_->Loop();
   }

   ~RpcServer() {
       executor_->Shutdown();
   }

private:
    std::shared_ptr<Executor> executor_ = std::make_shared<Executor>();
};

}

#endif
