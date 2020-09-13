#ifndef _SERVICE_HANDLE_H_
#define _SERVICE_HANDLE_H_


#include "dousi_future.h"

#include "core/submitter/submitter_runtime.h"
#include "core/common/function_traits.h"
#include "common/msgpack_utils.h"

#include <string>
#include <utility>

namespace dousi {


template <typename ReturnType>
class RemoteMethod;



class ServiceHandle {
public:
    explicit ServiceHandle(std::string service_name) :service_name_(std::move(service_name)) {}

    template<typename MethodType, typename... ArgTypes>
    auto Call(RemoteMethod<MethodType> method, ArgTypes... args)
            -> DousiFuture<typename FunctionTraits<MethodType>::ReturnType>;

private:
    std::string service_name_;
};


//////////////////////////////////////////// Implementations

template<typename MethodType, typename... ArgTypes>
auto ServiceHandle::Call(RemoteMethod<MethodType> method, ArgTypes... args)
            -> DousiFuture<typename FunctionTraits<MethodType>::ReturnType> {
    const auto object_id = SubmitterRuntime::GetInstance().RequestObjectID();

    msgpack::sbuffer sbuffer = common::PackArgsToBuffer(method.GetName(), std::forward<ArgTypes>(args)...);

    using ReturnType = typename FunctionTraits<MethodType>::ReturnType;
    std::string buffer {sbuffer.data(), sbuffer.size()};
    SubmitterRuntime::GetInstance().Submit(object_id, method.GetName(), buffer);
    return DousiFuture<ReturnType> { object_id };

}

}
#endif
