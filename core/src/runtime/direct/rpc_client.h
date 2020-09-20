#ifndef _DOUSI_RUNTIME_DIRECT_RPC_CLIENT_H
#define _DOUSI_RUNTIME_DIRECT_RPC_CLIENT_H

#include <memory>

#include "core/submitter/submitter.h"
#include "core/submitter/service_handle.h"

namespace dousi {

class RpcClient {
public:
    RpcClient(const std::string &server_address) {
        submitter_->Init(server_address);
    }

    ~RpcClient() {
        submitter_->Shutdown();
    }

    ServiceHandle GetService(const std::string &service_name) {
        return submitter_->GetService(service_name);
    }

private:
    std::shared_ptr<Submitter> submitter_ = std::make_shared<Submitter>();
};

}

#endif
