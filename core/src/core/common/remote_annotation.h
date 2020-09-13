#ifndef _CORE_COMMON_REMOTE_ANNOTATION_H_
#define _CORE_COMMON_REMOTE_ANNOTATION_H_

#include <string>
#include <utility>

namespace dousi {


// TODO(qwang): Use function traits.
template <typename MethodType>
struct RemoteMethod {
    RemoteMethod(std::string method_name, MethodType method)
            : method_name_(std::move(method_name)), method_(method) {
    }

    [[nodiscard]] std::string GetName() const { return method_name_; }

    MethodType GetMethod() { return method_; }

    std::string method_name_;

    MethodType method_;
};

/// The remote annotation.
#define Remote(METHOD) RemoteMethod<decltype(METHOD)>(std::string(NAMEOF(METHOD)), (METHOD))


}

#endif
