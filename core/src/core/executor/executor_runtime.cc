
#include "core/executor/executor_runtime.h"

#include <memory>

namespace dousi {

void ExecutorRuntime::DoAccept() {
    acceptor_->async_accept([this](boost::system::error_code error_code, asio_tcp::socket socket) {
        if (!error_code) {
            DOUSI_LOG(INFO) << "Succeed to accepted a connection.";
            uint64_t conn_id = this->RequestConnectionSessionID();
            // TODO(qwang): This conn_id should be reduced from the lambda captured and pass by value arg.
            auto conn_session = std::make_shared<executor::ConnectionSession>(
                    conn_id,
                    std::move(socket),
                    // This result could be removed.
                    /*callback=*/[this](uint64_t conn_id, const std::string &data, std::string &result) {
                        InvokeMethod(conn_id, data, result);
                    });
            conn_sessions_[conn_id] = conn_session;
            conn_session->Start();
        }
        DoAccept();
    });
}

}
