
#include "core/executor/executor.h"

#include <memory>

namespace dousi {

void Executor::DoAccept() {
//    std::shared_ptr<boost::asio::io_service> io_service_for_stream = std::make_shared<boost::asio::io_service>();
//    auto *work = new boost::asio::io_service::work  {*io_service_for_stream};
    std::shared_ptr<asio_tcp::socket> socket = std::make_shared<asio_tcp::socket>(*io_thread_pool_.GetNextIOService());
    acceptor_->async_accept(*socket, [this, socket](boost::system::error_code error_code) {
        if (!error_code) {
            DOUSI_LOG(INFO) << "Succeed to accepted a connection.";
            uint64_t stream_id = this->RequestStreamID();
            // TODO(qwang): This stream_id should be reduced from the lambda captured and pass by value arg.
            auto asio_stream = std::make_shared<AsioStream>(
                    stream_id,
                    std::move(*socket),
                    // This result could be removed.
                    /*callback=*/[this](uint64_t stream_id, uint32_t object_id, const std::shared_ptr<char> &buffer_ptr, const size_t &buffer_size) {
                        InvokeMethod(stream_id, object_id, buffer_ptr, buffer_size);

                    });
            std::lock_guard<std::mutex> lock {streams_mutex_};
            streams_[stream_id] = asio_stream;
            asio_stream->Start();

        }
        DoAccept();
    });
}

void Executor::LoopToWriteResponse() {
    while (true) {
        DousiResponse *response = nullptr;
        response_queue_.WaitAndPop(&response);

        std::shared_ptr<AsioStream> stream;
        {
            std::lock_guard<std::mutex> lock {streams_mutex_};
            auto it = streams_.find(response->stream_id_);
            assert(it != streams_.end());
            stream = it->second;
        }
        // Note that this result is already serialized since we should know the ReturnType of it.
        stream->Write(response->object_id_, response->result_);
        DOUSI_LOG(INFO) << "Method invoked, result is \"" << response->result_ << "\".";
        delete response;
    }
}

void Executor::PerformRequest(const DousiRequest &request) {
    std::function<void(const std::shared_ptr<msgpack::unpacked> &unpacked, std::string &)> method;
    std::string return_value_str;
    {
        // perform request.
        std::lock_guard<std::mutex> lock {mutex_};
        // TODO(qwang): This can be refined by concurrent hash map.
        method = registered_methods_[request.method_name_];
    }
    method(request.unpacked_, return_value_str);
    response_queue_.Push(new DousiResponse {request.object_id_, request.stream_id_, return_value_str});
}

void Executor::LoopToPerformRequest() {
    while (true) {
        DousiRequest *request = nullptr;
        request_queue_.WaitAndPop(&request);
        PerformRequest(*request);
        delete request;
    }
}

void Executor::InvokeMethod(uint64_t stream_id, uint32_t object_id, const std::shared_ptr<char> &buffer_ptr, const size_t &buffer_size) {
    std::shared_ptr<msgpack::unpacked> unpacked = std::make_shared<msgpack::unpacked>();
    msgpack::unpack(*unpacked, buffer_ptr.get(), buffer_size);
    auto tuple = unpacked->get().as<std::tuple<std::string>>();
    const auto method_name = std::get<0>(tuple);

    // If there has work thread pool, post the request to request queue.
    if (executor_options_.work_thread_num_ > 0) {
        request_queue_.Push(new DousiRequest {object_id, stream_id, std::move(unpacked), method_name});
    } else {
        PerformRequest(DousiRequest {object_id, stream_id, std::move(unpacked), method_name});
    }
}

}
