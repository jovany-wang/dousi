#ifndef _DOUSI_CORE_STREAM_SH_
#define _DOUSI_CORE_STREAM_SH_

#include "common/endpoint.h"


#include <string>

namespace dousi {

class AsioStream : public std::enable_shared_from_this<AsioStream> {
public:
    using InvocationCallbackType = std::function<
            void (uint64_t stream_id, uint32_t object_id, const std::shared_ptr<char> &buffer_ptr, const size_t &buffer_size)>;

    /**
     * The constructor of AsioStream.
     *
     * @param stream_id The id of this stream. This filed should be removed.
     * @param socket The actual socket to transfer data.
     * @param invocation_callback The callback to be invoked once we received a complete message.
     */
    AsioStream(uint64_t stream_id, asio_tcp::socket socket, InvocationCallbackType invocation_callback)
        : stream_id_(stream_id),socket_(std::move(socket)), invocation_callback_(std::move(invocation_callback)) {}

    ~AsioStream() {
        socket_.close();
    }

    void Start() {
        DoReadObjectID();
    }

    void Write(uint32_t object_id, const std::string &data);

private:

    void DoReadObjectID();

    void DoReadHeader(uint32_t object_id);

    void DoReadBody(uint32_t object_id, uint32_t body_size);

private:
    uint64_t stream_id_;

    asio_tcp::socket socket_;

    InvocationCallbackType invocation_callback_;
};

}
#endif
