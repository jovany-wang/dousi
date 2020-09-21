#ifndef _DOUSI_MASTER_H_
#define _DOUSI_MASTER_H_

#include "common/endpoint.h"
#include "common/logging.h"

#include <unordered_map>
#include <string>
#include <iostream>
#include <memory>
#include <vector>

#include <boost/asio.hpp>

#include "core/executor/dousi_service.h"
#include "core/executor/executor.h"
#include "core/common/remote_annotation.h"
#include "runtime/distributed/master/master_service.h"


namespace dousi {
namespace master {

/**
 * The master server is a standalone component that provides the service discovery for
 * the Dousi servers and clients.
 *
 * A Dousi server provided by user should be registered into master, so that the
 * client which wants to use this service can find the service location from master.
 */
class MasterServer {
public:
  MasterServer() = delete;

  explicit MasterServer(const std::string &listening_address) {
      executor_ = std::make_shared<Executor>();
      auto service = executor_->CreateService<MasterService>(/*service_name=*/"MasterService");
      service.RegisterMethod(dousi::Remote(&MasterService::RegisterService));
      service.RegisterMethod(dousi::Remote(&MasterService::GetAllEndpoints));
      executor_->Init(listening_address);
  }

  virtual ~MasterServer() = default;


private:

    std::shared_ptr<Executor> executor_ = nullptr;
};

} // namespace master
} // namespace dousi

#endif
