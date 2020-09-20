#include "core/submitter/submitter.h"

#include "core/submitter/service_handle.h"

namespace dousi {

ServiceHandle Submitter::GetService(const std::string &service_name) {
    return ServiceHandle(shared_from_this(), service_name);
}

}
