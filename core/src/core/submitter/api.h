#ifndef _CORE_SUBMITTER_API_H_
#define _CORE_SUBMITTER_API_H_

#include "core/submitter/service_handle.h"

namespace dousi {

inline static void Init2(const std::string &server_address) {
    SubmitterRuntime::GetInstance().Init(server_address);
}

// TODO(qwang): This should refined as a general type call.
inline ServiceHandle GetService(const std::string &service_name) {
    return SubmitterRuntime::GetInstance().GetService(service_name);
}

}

#endif
