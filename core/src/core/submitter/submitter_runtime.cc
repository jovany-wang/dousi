#include "core/submitter/submitter_runtime.h"

#include "core/submitter/service_handle.h"

namespace dousi {

ServiceHandle SubmitterRuntime::GetService(const std::string &service_name) {
    return ServiceHandle(service_name);
}

}
