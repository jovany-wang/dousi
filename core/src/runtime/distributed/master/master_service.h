#ifndef _DOUSI_RUNTIME_MASTER_SERVICE_
#define _DOUSI_RUNTIME_MASTER_SERVICE_

#include <string>
#include <unordered_map>

#include "core/executor/abstract_service.h"

class MasterService : public dousi::AbstractService {
public:
    bool RegisterService(
            const std::string &service_name,
            const std::string &service_address) {
        endpoints_[service_name] = service_address;
        return true;
    }

    // This method is used for test only. Once we enable the mock server in test,
    // we should remove this method.
    [[nodiscard]] std::unordered_map<std::string, std::string> GetAllEndpoints() const {
        return endpoints_;
    }

private:
    // The map that maps service name to its endpoint.
    // TODO(qwang): Change the 2nd parameter's type to `Endpoint`.
    std::unordered_map<std::string, std::string> endpoints_;
};

#endif
