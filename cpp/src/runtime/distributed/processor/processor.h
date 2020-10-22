#ifndef _DOUSI_DISTRIBTUED_PROCESSOR_H_
#define _DOUSI_DISTRIBTUED_PROCESSOR_H_

#include "core/submitter/submitter.h"
#include "core/executor/executor.h"
#include "runtime/distributed/master/master_client.h"

#include "common/logging.h"

namespace dousi {
namespace distributed {

class Processor final {

public:
    Processor(const std::string &master_address)
        :dousi_executor_(std::make_shared<Executor>()),
        master_client_(master_address) {
        processor_id_ = master_client_.SyncRequestProcessId();
        // TODO(qwang): Do not hardcode.
        dousi_executor_->Init("127.0.0.1:2000" + std::to_string(processor_id_));
        executor_thread_ = std::make_unique<std::thread>([this]() {
            dousi_executor_->Loop();
        });
    }

    ~Processor() {
        executor_thread_->detach();
    }

    template<typename ServiceType>
    DousiService<ServiceType> CreateService() {
        const auto service_name = std::string(NAMEOF_TYPE(ServiceType));
        auto ret = dousi_executor_->CreateService<ServiceType>(service_name);
        {
            // Register service to Master. This can be refined as async.
            master_client_.RegisterService(service_name, dousi_executor_->GetListeningAddr());
        }
        return ret;
    }

    ServiceHandle GetService(const std::string &service_name) {
        decltype(service_name_to_endpoints_local_cache_)::iterator it;
        {
            std::lock_guard<std::mutex> lock{local_cache_mutex_};
            it = service_name_to_endpoints_local_cache_.find(service_name);
        }
        if (it == service_name_to_endpoints_local_cache_.end()) {
            DOUSI_LOG(DEBUG) << "Failed to get the service from local cache, try to fetch service from Master.";
            {
                std::lock_guard<std::mutex> lock{local_cache_mutex_};
                // Update the local cache.
                service_name_to_endpoints_local_cache_ = master_client_.GetAllEndpoints();
            }
        }
        // Found the service from local cache.
        return InternalDoGetService(service_name);
    }


private:
    ServiceHandle InternalDoGetService(const std::string &service_name) {
        // This method assumes that the service must be found in the local cache.
        std::lock_guard<std::mutex> lock{local_cache_mutex_};
        auto it = service_name_to_endpoints_local_cache_.find(service_name);
        DOUSI_CHECK(it != service_name_to_endpoints_local_cache_.end());

        const auto dest_endpoint_str = it->second;
        auto submitter_it = dousi_submitters_.find(dest_endpoint_str);
        if (submitter_it == dousi_submitters_.end()) {
            auto submitter = std::make_shared<Submitter>();
            submitter->Init(dest_endpoint_str);
            dousi_submitters_[dest_endpoint_str] = std::move(submitter);
        }
        return dousi_submitters_[dest_endpoint_str]->GetService(service_name);
    }

private:
    int32_t processor_id_ = -1;

    std::mutex local_cache_mutex_;

    // service name to endpoints local cache.
    std::unordered_map<std::string, std::string> service_name_to_endpoints_local_cache_;

    // The map that maps the endpoint of the peer to the submitter to the peer.
    std::unordered_map<std::string, std::shared_ptr<Submitter>> dousi_submitters_;

    std::shared_ptr<Executor> dousi_executor_;

    master::MasterClient master_client_;

    std::unique_ptr<std::thread> executor_thread_;
};

}
}

#endif
