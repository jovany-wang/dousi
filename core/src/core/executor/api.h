#ifndef _CORE_EXECUTOR_API_H_
#define _CORE_EXECUTOR_API_H_

#include <string>

#include "core/executor/executor_runtime.h"

namespace dousi {
namespace executor {

//template <typename ServiceType>
//class DousiService;

/// Service related.
template<typename ServiceType>
inline DousiService<ServiceType> CreateService() {
    const auto service_name = std::string(NAMEOF_TYPE(ServiceType));
    return ExecutorRuntime::GetInstance().CreateService<ServiceType>(service_name);
}

static void Init(const std::string &master_address) {
    ExecutorRuntime::GetInstance().Init(master_address);
}

static void Shutdown() {
    ExecutorRuntime::GetInstance().Shutdown();
}

static void Loop() {
    ExecutorRuntime::GetInstance().Loop();
}


}
}


#endif
