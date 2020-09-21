#ifndef _SERVICE_HANDLE_H_
#define _SERVICE_HANDLE_H_


#include "dousi_future.h"

#include "core/submitter/submitter.h"
#include "core/common/function_traits.h"
#include "common/msgpack_utils.h"

#include <string>
#include <utility>

namespace dousi {


template <typename ReturnType>
class RemoteMethod;



class ServiceHandle {
public:
    ServiceHandle(ServiceHandle &&other)
            : service_name_(std::move(other.service_name_)),
            dousi_submitter_(std::move(other.dousi_submitter_)) {
    }

    ServiceHandle(std::shared_ptr<Submitter> dousi_submitter, std::string service_name)
            : dousi_submitter_(std::move(dousi_submitter)), service_name_(std::move(service_name)) {}

    template<typename MethodType, typename... ArgTypes>
    auto Call(RemoteMethod<MethodType> method, ArgTypes... args);

private:

    template<typename MethodType, typename... ArgTypes>
    auto CallWithMethodName(RemoteMethod<MethodType> method, ArgTypes... args);

    template<typename MethodType, typename ArgsTupleType>
    auto InternalCall(RemoteMethod<MethodType> method, ArgsTupleType &&args_tuple, NonVoidReturnTag unused)
            -> DousiFuture<typename FunctionTraits<MethodType>::ReturnType> {
        const auto object_id = dousi_submitter_->RequestObjectID();

        msgpack::sbuffer sbuffer = common::PackArgsToBuffer(std::forward<ArgsTupleType>(args_tuple));
        using ReturnType = typename FunctionTraits<MethodType>::ReturnType;
        std::string buffer {sbuffer.data(), sbuffer.size()};

        dousi_submitter_->Submit(object_id, method.GetName(), buffer);
        return DousiFuture<ReturnType> { dousi_submitter_, object_id };
    }

    template<typename MethodType, typename ArgsTupleType>
    auto InternalCall(RemoteMethod<MethodType> method, ArgsTupleType &&args_tuple, VoidReturnTag unused)
            -> DousiFuture<bool> {
        const auto object_id = dousi_submitter_->RequestObjectID();

        msgpack::sbuffer sbuffer = common::PackArgsToBuffer(args_tuple);

        std::string buffer {sbuffer.data(), sbuffer.size()};
        dousi_submitter_->Submit(object_id, method.GetName(), buffer);
        return DousiFuture<bool> { dousi_submitter_, object_id };
    }

private:
    std::string service_name_;

    // The submitter that to submit requests.
    std::shared_ptr<Submitter> dousi_submitter_;
};


//////////////////////////////////////////// Implementations

template<typename MethodType, typename... ArgTypes>
auto ServiceHandle::Call(RemoteMethod<MethodType> method, ArgTypes... args) {
    return CallWithMethodName(method, method.GetName(), args...);
}

template<typename MethodType, typename... ArgTypes>
auto ServiceHandle::CallWithMethodName(RemoteMethod<MethodType> method, ArgTypes... args) {
    using ReturnType = typename FunctionTraits<MethodType>::ReturnType;
    return InternalCall(
            method,
            std::forward_as_tuple(std::forward<ArgTypes>(args)...),
            typename VoidReturnTrait<ReturnType>::Tag {});
}

} // namespace dousi

#endif
