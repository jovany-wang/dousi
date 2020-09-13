#ifndef _DOUSI_H_
#define _DOUSI_H_

#include <string>

#include <nameof/nameof.hpp>

#include "core/common/remote_annotation.h"
#include "core/executor/executor_runtime.h"
#include "core/executor/dousi_service.h"
#include "core/submitter/service_handle.h"
namespace dousi {


template <typename ServiceType>
class DousiService;

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

#endif
