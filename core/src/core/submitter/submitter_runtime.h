#ifndef _CORE_SUBMITTER_RUNTIME_H_
#define _CORE_SUBMITTER_RUNTIME_H_

#include <string>
#include <unordered_map>
#include <atomic>
#include <thread>
#include <mutex>
#include <boost/asio.hpp>

#include "msgpack.hpp"
#include "common/logging.h"
#include "common/endpoint.h"

#include <nameof/nameof.hpp>


namespace dousi {

class ServiceHandle;

class SubmitterRuntime final {
public:
    static SubmitterRuntime &GetInstance() {
        static SubmitterRuntime submitter_runtime;
        return submitter_runtime;
    }

    ~SubmitterRuntime() {
        socket_.close();
        io_service_.stop();
        io_service_thread_->join();
    }

    SubmitterRuntime()
        : work_(io_service_), socket_(io_service_) {
    }

    void Submit(uint64_t object_id, const std::string &method_name, const std::string &buffer) {
        {
            // This is not a really header, it's object id of this request.
            DoWriteHeader(static_cast<uint32_t>(object_id), nullptr);
        }
        DoWriteHeader(buffer.size(), nullptr);
        // TODO(qwang): Confirm that the buffer is small that we call copy it.
        DoWriteBody(buffer);
    }

    void Init(const std::string &server_address) {
        DoConnect(Endpoint("127.0.0.1", 10001).Resolve(io_service_));
        // Async read from socket.
        DoAsyncRead();
        io_service_thread_ = std::make_unique<std::thread>([this]() {
            io_service_.run();
        });
    }

    void Shutdown() {

    }

    template<typename ObjectType>
    std::shared_ptr<ObjectType> GetObject(uint64_t object_id) {
        while(true) {
            auto it = cached_objects_.find(object_id);
            if (it != cached_objects_.end()) {
                break;
            }
            // TODO(qwang): This while should be refined in condition variable.
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        auto it = cached_objects_.find(object_id);
        assert(it != cached_objects_.end());

        std::string serialized_bytes = it->second;

        // decode for the serialized_bytes and return.
        msgpack::unpacked unpacked;
        msgpack::unpack(unpacked, serialized_bytes.data(), serialized_bytes.size());
        auto object = unpacked.get().as<ObjectType>();

        return std::make_shared<ObjectType>(object);
    }

    uint64_t RequestObjectID() {
        const auto ret = curr_object_id_.fetch_add(1, std::memory_order_relaxed);
        return ret;
    }

    static ServiceHandle GetService(const std::string &service_name);

private:
    void DoConnect(const asio_tcp::resolver::results_type &endpoints) {
        boost::asio::connect(socket_, endpoints);
        DOUSI_LOG(DEBUG) << "Succeeded to connect message to server.";
    }


    void DoWriteHeader(uint32_t body_size, const std::function<void()> &done_callback);

    void DoWriteBody(const std::string &buffer);

    void DoAsyncRead() {
        DoAsyncReadObjectID();
    }

    void DoAsyncReadObjectID();

    void DoAsyncReadHeader(uint32_t object_id);

    void DoAsyncReadBody(uint32_t object_id, uint32_t body_size);

private:

    std::atomic<uint64_t> curr_object_id_ = 0;

    // The mutex that protects the following map.
    std::mutex mutex_;

    // object_id -> serialized bytes of the object.
    std::unordered_map<uint64_t, std::string> cached_objects_;

    boost::asio::io_service io_service_ = {};

    boost::asio::io_service::work work_;

    // The thread that runs io service.
    std::unique_ptr<std::thread> io_service_thread_ = nullptr;

    // The mutex that protects socket_.
    std::mutex socket_mutex_;


    asio_tcp::socket socket_;

};

}
#endif
