#ifndef _DOUSI_CORE_STREAM_SH_
#define _DOUSI_CORE_STREAM_SH_

#include "common/endpoint.h"


#include <string>

namespace dousi {

class AsioStream : public std::enable_shared_from_this<AsioStream> {
public:
    using InvocationCallbackType = std::function<
            void (uint64_t conn_id, const std::string &data, std::string &result)>;

    AsioStream(uint64_t stream_id, asio_tcp::socket socket, InvocationCallbackType invocation_callback)
        : stream_id_(stream_id),socket_(std::move(socket)), invocation_callback_(std::move(invocation_callback)) {}

    ~AsioStream() {
        socket_.close();
    }

    void Start() {
        boost::asio::post([this]() {this->DoReadObjectID(); });
    }

    void Write(const uint32_t object_id, const std::string &data);

private:
    void DoReadObjectID();

    void DoReadHeader(uint32_t object_id);

    void DoReadBody(uint32_t object_id, uint32_t body_size);

    void DoWriteObjectID(uint32_t object_id, const std::function<void()> &done_callback);

    void DoWriteHeader(uint32_t data_size, const std::function<void()> &done_callback);

    void DoWriteBody(const std::string &data, const std::function<void()> &done_callback);

private:
    uint64_t stream_id_;

    asio_tcp::socket socket_;

    InvocationCallbackType invocation_callback_;
};

}
#endif
