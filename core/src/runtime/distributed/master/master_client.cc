#include "master_client.h"
#include "core/common/remote_annotation.h"
#include "runtime/distributed/master/master_service.h"

#include <nameof/nameof.hpp>

namespace dousi {
namespace master {

void MasterClient::RegisterService(const std::string &service_name,
    const std::string &service_address) {
    auto future = master_service_handle_->Call(
            dousi::Remote(&MasterService::RegisterService),
            service_name, service_address);
    // TODO(qwang): Rename to AsyncXXX and add callback on this.
}

std::unordered_map<std::string, std::string> MasterClient::GetAllEndpoints() {
    auto future = master_service_handle_->Call(
            dousi::Remote(&MasterService::GetAllEndpoints));
    return *future.Get();
}


void MasterClient::FetchService(const std::string &service_name,
    const std::function<void(bool ok, const std::string &address)> &callback) {

}

} // namespace master
} // namespace dousi
