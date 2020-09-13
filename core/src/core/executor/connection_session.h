#ifndef _CORE_EXECUTOR_CONNECTION_SESSION_
#define _CORE_EXECUTOR_CONNECTION_SESSION_

#include <memory>
#include <boost/asio.hpp>

#include "common/endpoint.h"

//namespace dousi {
//class ExecutorRuntime;
//}

namespace dousi {
namespace executor {

class ConnectionSession : public std::enable_shared_from_this<ConnectionSession> {
public:
    ConnectionSession(uint64_t conn_id,
                      asio_tcp::socket socket,
                      std::function<void (uint64_t conn_id, const std::string &data, std::string &result)> invocation_callback)
            : conn_id_(conn_id), socket_(std::move(socket)), invocation_callback_(std::move(invocation_callback)) {}

    virtual ~ConnectionSession() {
        socket_.close();
    }

    void Start() {
        boost::asio::post([this]() {DoReadObjectID();});
    }

    void Write(const std::string &data);

private:
    void DoReadObjectID();

    void DoReadHeader(uint32_t object_id);

    void DoReadBody(uint32_t object_id, uint32_t body_size);

private:
    // The reference to the corresponding executor runtime.
    // TODO(qwang): This should abstracted and refined.
//    ExecutorRuntime *executor_runtime_;

    uint64_t conn_id_;

    // The socket handle for the client that connected to this server.
    asio_tcp::socket socket_;

    std::function<void (uint64_t conn_id, const std::string &data, std::string &result)> invocation_callback_;
};

}
}

#endif
