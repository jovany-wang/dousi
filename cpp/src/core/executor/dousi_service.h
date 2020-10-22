#ifndef _DOUSI_SERVICE_H_
#define _DOUSI_SERVICE_H_

#include "core/executor/executor.h"
#include "common/logging.h"

#include <string>


namespace dousi {

template<typename ServiceType>
class DousiService : std::enable_shared_from_this<DousiService<ServiceType>> {
public:

    DousiService(std::shared_ptr<Executor> dousi_executor, std::shared_ptr<ServiceType> service_object)
        : dousi_executor_(std::move(dousi_executor)), service_object_(std::move(service_object)) {}

    [[nodiscard]] std::string GetName() const { return service_name_; }

    ServiceType &GetServiceObjectRef() {
        return *service_object_;
    }

    template<typename MethodType>
    void RegisterMethod(RemoteMethod<MethodType> remote_method) {
        DOUSI_LOG(INFO) << "Registering method:" << remote_method.GetName() << ".";
        dousi_executor_->RegisterMethod(this, remote_method);
    }

private:
    const std::string service_name_;

    const std::shared_ptr<ServiceType> service_object_ = nullptr;

    // The DousiExecutor that the service belong to.
    std::shared_ptr<dousi::Executor> dousi_executor_ = nullptr;
};

}

#endif
