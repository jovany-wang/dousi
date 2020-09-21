#ifndef _DOUSI_MASTER_MASTER_CLIENT_H_
#define _DOUSI_MASTER_MASTER_CLIENT_H_

#include "common/endpoint.h"
#include "common/logging.h"

#include <msgpack.hpp>

#include <iostream>

namespace dousi {
namespace master {

class MasterClient {
public:

  MasterClient(const std::string &master_server_address) {
      submitter_ = std::make_shared<Submitter>();
      submitter_->Init(master_server_address);
      auto master_service_handle = submitter_->GetService("MasterService");
      master_service_handle_ = std::make_shared<ServiceHandle>(std::move(master_service_handle));
  }

  virtual ~MasterClient() = default;

  /**
   * Register a Dousi RPC service to the master server.
   *
   * @param service_name The service name to be registered.
   * @param service_address The address of this service.
   */
  void RegisterService(const std::string &service_name,
                       const std::string &service_address);

  /**
   * Get the service routing from master server.
   */
  void FetchService(const std::string &service_name,
      const std::function<void(bool ok, const std::string &address)> &callback);

private:


private:
    std::shared_ptr<Submitter> submitter_ = nullptr;

    std::shared_ptr<ServiceHandle> master_service_handle_ = nullptr;
};

} // namespace master
} // namespace dousi

#endif
