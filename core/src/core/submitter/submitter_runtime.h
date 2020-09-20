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

#include "core/stream/stream.h"

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
//        stream_.Close();
        io_service_.stop();
        io_service_thread_->join();
    }

    SubmitterRuntime()
        : work_(io_service_) {
    }

    void Submit(uint64_t object_id, const std::string &method_name, const std::string &buffer) {
        stream_->Write(object_id, buffer);
    }

    void Init(const std::string &server_address) {
        DoConnect(Endpoint(server_address).Resolve(io_service_));
        // Async read from stream.
        stream_->Start();
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
        asio_tcp::socket socket(io_service_);
        boost::asio::connect(socket, endpoints);
        stream_ = std::make_shared<AsioStream>(
                /*unused*/0,
                std::move(socket),
                /*invocation_callback=*/[this](uint64_t stream_id, uint32_t object_id, const std::string &data, std::string &result) {
                    // TODO(qwang): This deserialized should be refined as a separated io thread.
                    cached_objects_[object_id] = data;
                });
        DOUSI_LOG(DEBUG) << "Succeeded to connect message to server.";
    }


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

    std::shared_ptr<AsioStream> stream_ = nullptr;

};

}
#endif
