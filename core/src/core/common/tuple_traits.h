#ifndef _CORE_COMMON_H_
#define _CORE_COMMON_H_

#include <string>

namespace dousi {

/// These 2 traits help to trim the first argument with its type, because the first argument is
/// the method name instead of a real argument.
template<typename MethodType, typename ServiceOriginalType, typename FirstArgType, typename... Args>
static auto TraitAndCall(
        const MethodType &method, ServiceOriginalType &service_object, std::tuple<FirstArgType, Args...> tuple) {
    return TraitAndCall2(method, service_object, std::make_index_sequence<sizeof...(Args)>{}, std::move(tuple));
}

template<typename MethodType, typename ServiceOriginalType, typename FirstArgType, typename... Args>
static auto TraitAndCallVoidReturn(
        const MethodType &method, ServiceOriginalType &service_object, std::tuple<FirstArgType, Args...> tuple) {
    TraitAndCall2VoidReturn(method, service_object, std::make_index_sequence<sizeof...(Args)>{}, std::move(tuple));
}

template<typename MethodType, typename ServiceOriginalType, size_t... Index, typename... Args>
inline static auto TraitAndCall2(
        const MethodType &method,
        /// Note that this `service_object` should be passed by reference to make sure
        /// the methods will be performed on one service instance. The issue of that is
        /// https://github.com/jovany-wang/dousi/issues/12
        ServiceOriginalType &service_object,
        const std::index_sequence<Index ...> &, std::tuple<std::string, Args...> tuple) {
    return (service_object.*method)(std::move(std::get<Index + 1>(tuple)) ...);
}

template<typename MethodType, typename ServiceOriginalType, size_t... Index, typename... Args>
inline static auto TraitAndCall2VoidReturn(
        const MethodType &method,
        ServiceOriginalType &service_object,
        const std::index_sequence<Index ...> &, std::tuple<std::string, Args...> tuple) {
    (service_object.*method)(std::move(std::get<Index + 1>(tuple)) ...);
}

template<typename ServiceType>
class DousiService;

// TODO(qwang): Use tag dispatching.
template<typename ReturnType>
struct InvokeHelper {
    // This could be refined as a lambda.
    template<typename MethodType, typename ServiceOriginalType>
    static void Invoke(
            const MethodType &method,
            DousiService<ServiceOriginalType> *service_instance,
            const std::shared_ptr<msgpack::unpacked> &unpacked,
            std::string &result) {
        using MethodNameWithArgsTupleTypes = typename FunctionTraits<MethodType>::MethodNameWithArgsTuple;
        auto method_name_and_args_tuple = unpacked->get().as<MethodNameWithArgsTupleTypes>();

        auto ret = TraitAndCall(method, service_instance->GetServiceObjectRef(), method_name_and_args_tuple);

        // Serialize result.
        msgpack::sbuffer buffer(1024);
        msgpack::pack(buffer, ret);
        result = {buffer.data(), buffer.size()};
    }
};

template<>
struct InvokeHelper<void> {
    // This could be refined as a lambda.
    template<typename MethodType, typename ServiceOriginalType>
    static void Invoke(
            const MethodType &method,
            DousiService<ServiceOriginalType> *service_instance,
            const std::shared_ptr<msgpack::unpacked> &unpacked,
            std::string &result) {
        using MethodNameWithArgsTupleTypes = typename FunctionTraits<MethodType>::MethodNameWithArgsTuple;
        auto method_name_and_args_tuple = unpacked->get().as<MethodNameWithArgsTupleTypes>();

        TraitAndCallVoidReturn(method, service_instance->GetServiceObjectRef(), method_name_and_args_tuple);
        DOUSI_LOG(INFO) << "The type of the result is void.";
    }
};

}

#endif
