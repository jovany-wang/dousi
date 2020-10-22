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

enum class ClientLanguage {
    CPP = 0,
    JAVA = 1,
};

class ServiceHandle;

using RpcReturnedCallback = std::function<void(uint32_t object_id,
        const std::shared_ptr<char> &buffer_ptr, const size_t buffer_size)>;


class Submitter : public std::enable_shared_from_this<Submitter> {
public:

    Submitter()
        : Submitter(ClientLanguage::CPP, nullptr) {}

    Submitter(ClientLanguage client_language, RpcReturnedCallback on_rpc_returned_callback)
        : work_(io_service_),
        client_language_(client_language),
        on_rpc_returned_callback_(std::move(on_rpc_returned_callback)) {}

    ~Submitter() {
//        stream_.Close();
        io_service_.stop();
        if (io_service_thread_ != nullptr) {
            io_service_thread_->join();
        }
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
            {
                std::lock_guard<std::mutex> lock_guard{mutex_};
                auto it = cached_objects_.find(object_id);
                if (it != cached_objects_.end()) {
                    break;
                }
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

    ServiceHandle GetService(const std::string &service_name);

private:
    void DoConnect(const asio_tcp::resolver::results_type &endpoints) {
        asio_tcp::socket socket(io_service_);
        boost::asio::connect(socket, endpoints);
        stream_ = std::make_shared<AsioStream>(
                /*unused*/0,
                std::move(socket),
                // TODO(qwang): This invocation_callback should be refined as a non-param lambda.
                /*invocation_callback=*/[this](uint64_t stream_id, uint32_t object_id, const std::shared_ptr<char> &buffer_ptr, const size_t buffer_size) {
                    // TODO(qwang): This deserialized should be refined as a separated io thread.
                    // TODO(qwang): This should not be written here, `Submitter` just provide a callback way
                    // for the user to pass a callback to indicate how to handle the result received.

                    if (client_language_ == ClientLanguage::JAVA) {
                        DOUSI_CHECK(on_rpc_returned_callback_ != nullptr)
                                << "on_rpc_returned_callback_ should not be null in Java Client.";
                        on_rpc_returned_callback_(object_id, buffer_ptr, buffer_size);
                    } else {
                        std::lock_guard<std::mutex> lock_guard {mutex_};
                        cached_objects_[object_id] = std::string(buffer_ptr.get(), buffer_size);
                    }
                });
        DOUSI_LOG(DEBUG) << "Succeeded to connect to server.";
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

    ClientLanguage client_language_ = ClientLanguage::CPP;

    RpcReturnedCallback  on_rpc_returned_callback_ = nullptr;
};

}
#endif
