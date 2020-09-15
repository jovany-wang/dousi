
#include "core/executor/executor_runtime.h"

#include <memory>

namespace dousi {

void ExecutorRuntime::DoAccept() {
    acceptor_->async_accept([this](boost::system::error_code error_code, asio_tcp::socket socket) {
        if (!error_code) {
            DOUSI_LOG(INFO) << "Succeed to accepted a connection.";
            uint64_t stream_id = this->RequestStreamID();
            // TODO(qwang): This stream_id should be reduced from the lambda captured and pass by value arg.
            auto asio_stream = std::make_shared<AsioStream>(
                    stream_id,
                    std::move(socket),
                    // This result could be removed.
                    /*callback=*/[this](uint64_t stream_id, uint32_t object_id, const std::string &data, std::string &result) {
                        InvokeMethod(stream_id, object_id, data, result);

                    });
            streams_[stream_id] = asio_stream;
            asio_stream->Start();
        }
        DoAccept();
    });
}

}
