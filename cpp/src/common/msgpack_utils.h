#ifndef _COMMON_MSGPACK_UTILS_H_
#define _COMMON_MSGPACK_UTILS_H_

#include "msgpack.hpp"

namespace dousi {
namespace common {

template<typename ArgsTupleType>
inline static msgpack::sbuffer PackArgsToBuffer(ArgsTupleType&& args_tuple) {
    msgpack::sbuffer buffer(1024);
    msgpack::pack(buffer, args_tuple);
    return buffer;
}

}
}

#endif
