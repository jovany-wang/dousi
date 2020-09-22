
#include "core/executor/executor.h"

#include <memory>

namespace dousi {

void Executor::DoAccept() {
    acceptor_->async_accept([this](boost::system::error_code error_code, asio_tcp::socket socket) {
        if (!error_code) {
            DOUSI_LOG(INFO) << "Succeed to accepted a connection.";
            uint64_t stream_id = this->RequestStreamID();
            // TODO(qwang): This stream_id should be reduced from the lambda captured and pass by value arg.
            auto asio_stream = std::make_shared<AsioStream>(
                    stream_id,
                    std::move(socket),
                    // This result could be removed.
                    /*callback=*/[this](uint64_t stream_id, uint32_t object_id, const std::shared_ptr<char> &buffer_ptr, const size_t &buffer_size) {
                        InvokeMethod(stream_id, object_id, buffer_ptr, buffer_size);

                    });
            std::lock_guard<std::mutex> lock {streams_mutex_};
            streams_[stream_id] = asio_stream;

            std::thread read_thread {[asio_stream]() { asio_stream->Start(); }};
            read_thread_pool_.emplace_back(std::move(read_thread));

        }
        DoAccept();
    });
}

}
