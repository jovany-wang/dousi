
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

void Executor::LoopToWriteResponse() {
    while (true) {
        DousiResponse response;
        response_queue_.WaitAndPop(&response);

        std::shared_ptr<AsioStream> stream;
        {
            std::lock_guard<std::mutex> lock {streams_mutex_};
            auto it = streams_.find(response.stream_id_);
            assert(it != streams_.end());
            stream = it->second;
        }
        // Note that this result is already serialized since we should know the ReturnType of it.
        stream->Write(response.object_id_, response.result_);
        DOUSI_LOG(INFO) << "Method invoked, result is \"" << response.result_ << "\".";
    }
}

void Executor::PerformRequest(const DousiRequest &request) {
    std::function<void(const std::shared_ptr<char>&, const size_t buffer_size, std::string &)> method;
    std::string return_value_str;
    {
        // perform request.
        std::lock_guard<std::mutex> lock {mutex_};
        // TODO(qwang): This can be refined by concurrent hash map.
        method = registered_methods_[request.method_name_];
    }
    method(request.buffer_ptr_, request.buffer_size_, return_value_str);
    response_queue_.Push(DousiResponse {request.object_id_, request.stream_id_, return_value_str});
}

void Executor::LoopToPerformRequest() {
    while (true) {
        DousiRequest request;
        request_queue_.WaitAndPop(&request);
        PerformRequest(request);
    }
}

void Executor::InvokeMethod(uint64_t stream_id, uint32_t object_id, const std::shared_ptr<char> &buffer_ptr, const size_t &buffer_size) {
    msgpack::unpacked unpacked;
    msgpack::unpack(unpacked, buffer_ptr.get(), buffer_size);
    auto tuple = unpacked.get().as<std::tuple<std::string>>();
    const auto method_name = std::get<0>(tuple);

    // If there has work thread pool, post the request to request queue.
    if (executor_options_.work_thread_num_ > 0) {
        request_queue_.Push(DousiRequest {object_id, stream_id, buffer_ptr, buffer_size, method_name});
    } else {
        PerformRequest(DousiRequest {object_id, stream_id, buffer_ptr, buffer_size, method_name});
    }
}

}
