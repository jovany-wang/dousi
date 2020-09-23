#ifndef _DOUSI_REQUEST_H_
#define _DOUSI_REQUEST_H_

#include <string>
#include <utility>
#include <msgpack.hpp>

namespace dousi {

struct DousiRequest {
    DousiRequest() {}

    DousiRequest(uint32_t object_id, uint64_t stream_id, std::shared_ptr<msgpack::unpacked> unpacked, std::string method_name)
            : object_id_(object_id),
            stream_id_(stream_id),
            unpacked_(std::move(unpacked)),
            method_name_(std::move(method_name)) {}

    DousiRequest &operator=(const DousiRequest &other) {
        object_id_ = other.object_id_;
        stream_id_ = other.stream_id_;
        unpacked_ = other.unpacked_;
        method_name_ = other.method_name_;
        return *this;
    }

    uint32_t object_id_;
    uint64_t stream_id_;
    std::shared_ptr<msgpack::unpacked> unpacked_;
    std::string method_name_;
};

struct DousiResponse {
    DousiResponse() {}

    DousiResponse(uint32_t object_id, uint64_t stream_id, std::string result)
        : object_id_(object_id),
        stream_id_(stream_id),
        result_(std::move(result)) {}

    DousiResponse &operator=(const DousiResponse &other) {
        object_id_ = other.object_id_;
        stream_id_ = other.stream_id_;
        result_ = other.result_;
        return *this;

    }
    uint32_t object_id_;
    uint64_t stream_id_;
    std::string result_;
};

}

#endif
