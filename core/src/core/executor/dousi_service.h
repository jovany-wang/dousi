#ifndef _DOUSI_SERVICE_H_
#define _DOUSI_SERVICE_H_

#include "core/executor/executor_runtime.h"
#include <string>


namespace dousi {

template<typename ServiceType>
class DousiService : std::enable_shared_from_this<DousiService<ServiceType>> {
public:

    explicit DousiService(std::shared_ptr<ServiceType> service_object) : service_object_(service_object) {}

    std::string GetName() const { return service_name_; }

    ServiceType &GetServiceObjectRef() {
        return *service_object_;
    }

    template<typename MethodType>
    void RegisterMethod(RemoteMethod<MethodType> remote_method) {
        DOUSI_LOG(INFO) << "Registering method:" << remote_method.GetName() << ".";
        ExecutorRuntime::GetInstance().RegisterMethod(this, remote_method);
    }

private:
    const std::string service_name_;

    const std::shared_ptr<ServiceType> service_object_ = nullptr;
};

}

#endif
