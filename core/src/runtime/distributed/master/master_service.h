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
        std::lock_guard<std::mutex> lock {endpoints_mutex_};
        endpoints_[service_name] = service_address;
        DOUSI_LOG(DEBUG) << "Service registered [ " << service_name << ", " << service_address << " ].";
        return true;
    }

    // This method is used for test only. Once we enable the mock server in test,
    // we should remove this method.
    [[nodiscard]] std::unordered_map<std::string, std::string> GetAllEndpoints() const {
        std::lock_guard<std::mutex> lock {endpoints_mutex_};
        return endpoints_;
    }

    int32_t RequestProcessorId() {
        const auto processor_id = curr_processor_id_.fetch_add(1, std::memory_order_relaxed);
        return processor_id;
    }


private:
    std::atomic<int32_t> curr_processor_id_ = 0;

    // The map that maps service name to its endpoint.
    // TODO(qwang): Change the 2nd parameter's type to `Endpoint`.
    std::unordered_map<std::string, std::string> endpoints_;

    // The mutex that protects endpoints above.
    mutable std::mutex endpoints_mutex_;
};

#endif
