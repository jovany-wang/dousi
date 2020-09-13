#ifndef _COMMON_MSGPACK_UTILS_H_
#define _COMMON_MSGPACK_UTILS_H_

#include "msgpack.hpp"

namespace dousi {
namespace common {

template<typename... Args>
inline static msgpack::sbuffer PackArgsToBuffer(Args&&... args) {
    msgpack::sbuffer buffer(1024);
    msgpack::pack(buffer, std::forward_as_tuple(std::forward<Args>(args)...));
    return buffer;
}

}
}

#endif
