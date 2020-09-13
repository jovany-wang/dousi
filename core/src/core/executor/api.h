#ifndef _CORE_EXECUTOR_API_H_
#define _CORE_EXECUTOR_API_H_

#include <string>

#include "core/executor/executor_runtime.h"

namespace dousi {
namespace executor {

inline static void Init(std::string master_address) {
    ExecutorRuntime::GetInstance().Init(master_address);

}

inline static void Loop() {
    ExecutorRuntime::GetInstance().Loop();
}

}
}


#endif
