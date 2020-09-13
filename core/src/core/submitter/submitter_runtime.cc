#include "core/submitter/submitter_runtime.h"

#include "core/submitter/service_handle.h"

namespace dousi {

ServiceHandle SubmitterRuntime::GetService(const std::string &service_name) {
    return ServiceHandle(service_name);
}

void SubmitterRuntime::DoWriteHeader(uint32_t body_size, const std::function<void()> &done_callback) {
    boost::asio::post(io_service_, [this, done_callback, body_size]() {
        char header[sizeof(body_size)];
        memcpy(header, &body_size, sizeof(body_size));
        // TODO(qwang): Remove this and make this is thread safe.
        std::lock_guard<std::mutex> guard(socket_mutex_);
        boost::asio::async_write(
                socket_,
                boost::asio::buffer(header, sizeof(body_size)),
                [this, done_callback, body_size](boost::system::error_code error_code, size_t) {
                    if (error_code) {
                        socket_.close();
                        DOUSI_LOG(INFO) << "Failed to write header to server with error code:" << error_code;
                    } else {
                        DOUSI_LOG(DEBUG) << "Succeeded to write header to server, header=" << body_size;
                        if (done_callback != nullptr) {
                            done_callback();
                        }
                    }
                });
    });
}

void SubmitterRuntime::DoWriteBody(const std::string &buffer) {
    // Copy str by value here.
    // TODO(qwang): Make this buffer as a ref counter buffer implementation.
    boost::asio::post(io_service_, [this, buffer]() {
        std::lock_guard<std::mutex> guard(socket_mutex_);
        boost::asio::async_write(
                socket_,
                boost::asio::buffer(buffer.data(), buffer.size()),
                [this](boost::system::error_code error_code, size_t /*length*/) {
                    if (error_code) {
                        socket_.close();
                        DOUSI_LOG(INFO) << "Failed to write message to server with error code:" << error_code.message();
                    } else {
                        DOUSI_LOG(DEBUG) << "Succeeded to write message to server.";
                    }
                });
    });
}

void SubmitterRuntime::DoAsyncReadObjectID() {
    std::shared_ptr<uint32_t> object_id_ptr = std::make_shared<uint32_t>();
    constexpr size_t HEADER_SIZE = sizeof(*object_id_ptr);
    // Read head first.
    // We should make sure the endian here. Otherwise the value will be incorrect.
    boost::asio::async_read(
            socket_,
            boost::asio::buffer(reinterpret_cast<char *>(object_id_ptr.get()), HEADER_SIZE),
            [this, object_id_ptr/* , self*/](boost::system::error_code error_code, size_t length) {
                // ASSERT(length == HEADER_SIZE);
                if (error_code) {
                    DOUSI_LOG(INFO) << "Failed to receive object_id with error code: " << error_code.message();
                    return;
                }
                DOUSI_LOG(INFO) << "Succeeded to receive the object_id: " << *object_id_ptr;
                DoAsyncReadHeader(*object_id_ptr);
            });
}

void SubmitterRuntime::DoAsyncReadHeader(uint32_t object_id) {
    std::shared_ptr<uint32_t> body_size_ptr = std::make_shared<uint32_t>();
    constexpr size_t HEADER_SIZE = sizeof(*body_size_ptr);
    // Read head first.
    // We should make sure the endian here. Otherwise the value will be incorrect.
    boost::asio::async_read(
            socket_,
            boost::asio::buffer(reinterpret_cast<char *>(body_size_ptr.get()), HEADER_SIZE),
            [this, object_id, body_size_ptr/* , self*/](boost::system::error_code error_code, size_t length) {
                // ASSERT(length == HEADER_SIZE);
                if (error_code) {
                    DOUSI_LOG(INFO) << "Failed to receive header with error code: " << error_code;
                    return;
                }
                DOUSI_LOG(INFO) << "Succeeded to receive the header: " << *body_size_ptr;
                DoAsyncReadBody(object_id, *body_size_ptr);
            });
}

void SubmitterRuntime::DoAsyncReadBody(uint32_t object_id, uint32_t body_size) {
    std::shared_ptr<char> buffer_ptr(new char[body_size], std::default_delete<char[]>());

    boost::asio::async_read(
            socket_,
            boost::asio::buffer(buffer_ptr.get(), body_size),
            [buffer_ptr, object_id, this/* , self*/](boost::system::error_code error_code, size_t length) {
                if (error_code) {
                    DOUSI_LOG(INFO) << "Failed to receive body with error code: " << error_code.message();
                    return;
                }
                std::string message(buffer_ptr.get(), length);
                // TODO(qwang): This deserialized should be refined as a separated io thread.
                cached_objects_[object_id] = message;
                DoAsyncReadObjectID();
            });
}

}
