
#include "stream/stream.h"
#include "common/logging.h"

#include <vector>

namespace dousi {


void AsioStream::DoReadObjectID() {
    // This self is used for the case that this self could be destroyed before the lambda performed.
    auto self(shared_from_this());
    std::shared_ptr<uint32_t> object_id_ptr = std::make_shared<uint32_t>();
    constexpr size_t HEADER_SIZE = sizeof(*object_id_ptr);
    // Read head first.
    // We should make sure the endian here. Otherwise the value will be incorrect.
    boost::asio::async_read(
            socket_,
            boost::asio::buffer(reinterpret_cast<char *>(object_id_ptr.get()), HEADER_SIZE),
            [this, object_id_ptr, self](boost::system::error_code error_code, size_t length) {
                // ASSERT(length == HEADER_SIZE);
                if (error_code) {
                    DOUSI_LOG(INFO) << "Failed to receive object_id with error code: " << error_code.message();
                    return;
                }
                DOUSI_LOG(INFO) << "Succeeded to receive the object_id : " << *object_id_ptr;
                DoReadHeader(*object_id_ptr);
            });
}

void AsioStream::DoReadHeader(uint32_t object_id) {
    // This self is used for the case that this self could be destroyed before the lambda performed.
    auto self(shared_from_this());
    std::shared_ptr<uint32_t> body_size_ptr = std::make_shared<uint32_t>();
    constexpr size_t HEADER_SIZE = sizeof(*body_size_ptr);
    // Read head first.
    // We should make sure the endian here. Otherwise the value will be incorrect.
    boost::asio::async_read(
            socket_,
            boost::asio::buffer(reinterpret_cast<char *>(body_size_ptr.get()), HEADER_SIZE),
            [this, object_id, body_size_ptr, self](boost::system::error_code error_code, size_t length) {
                // ASSERT(length == HEADER_SIZE);
                if (error_code) {
                    DOUSI_LOG(INFO) << "Failed to receive header with error code: " << error_code;
                    return;
                }
                DOUSI_LOG(INFO) << "Succeeded to receive the header: " << *body_size_ptr;
                DoReadBody(object_id, *body_size_ptr);
            });
}

void AsioStream::DoReadBody(uint32_t object_id, uint32_t body_size) {
    auto self {shared_from_this()};
    std::shared_ptr<char> buffer_ptr(new char[body_size], std::default_delete<char[]>());

    boost::asio::async_read(
            socket_,
            boost::asio::buffer(buffer_ptr.get(), body_size),
            [buffer_ptr, this, object_id, self](boost::system::error_code error_code, size_t length) {
                if (error_code) {
                    DOUSI_LOG(INFO) << "Failed to receive body with error code: " << error_code.message();
                    return;
                }

                std::string data(buffer_ptr.get(), length);
                DOUSI_LOG(INFO) << "Succeeded to receive the body: " << data;
                std::string result;
                invocation_callback_(stream_id_, object_id, data, result);
//                Write(object_id, result);
                DoReadObjectID();
            });
}

void AsioStream::Write(uint32_t object_id, const std::string &data) {
    // TODO(qwang): Use buffer to avoid this copy.
    /// Note that we shouldn't invoke `DoWriteHeader` in the first `done_callback`.
    /// Otherwise the `multiple_calls_with_no_wait.cc` couldn't pass.
    auto object_id_ptr = std::make_shared<uint32_t>(object_id);
    auto header_ptr  = std::make_shared<uint32_t>(data.size());
    auto data_ptr = std::make_shared<std::string>(data);
    auto self = this->shared_from_this();
    boost::asio::async_write(
            socket_,
            std::vector<boost::asio::const_buffer> {
                boost::asio::buffer(object_id_ptr.get(), sizeof(object_id)),
                boost::asio::buffer(header_ptr.get(), sizeof(uint32_t)),
                boost::asio::buffer(data_ptr->data(), data_ptr->size())
                },
                // Pass `self` and these shared_ptrs to guarantee that the data get freed after being used.
            [self, object_id_ptr, header_ptr, data_ptr, this](boost::system::error_code error_code, size_t) {
                if (error_code) {
                    socket_.close();
                    DOUSI_LOG(INFO) << "Failed to write message to peer with error code:" << error_code;
                } else {
                    DOUSI_LOG(DEBUG) << "Succeeded to write a message to peer, header=" << *header_ptr;
                }
            });
}

}
