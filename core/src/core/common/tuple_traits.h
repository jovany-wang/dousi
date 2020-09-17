#ifndef _CORE_COMMON_H_
#define _CORE_COMMON_H_

#include <string>

namespace dousi {

/// These 2 traits help to trim the first argument with its type, because the first argument is
/// the method name instead of a real argument.
template <typename MethodType, typename ServiceOriginalType, typename FirstArgType, typename... Args>
static auto TraitAndCall(
        const MethodType& method, ServiceOriginalType &service_object, std::tuple<FirstArgType, Args...> tuple) {
    return TraitAndCall2(method, service_object, std::make_index_sequence<sizeof...(Args)>{}, std::move(tuple));
}

template <typename MethodType, typename ServiceOriginalType, typename FirstArgType, typename... Args>
static auto TraitAndCallVoidReturn(
        const MethodType& method, ServiceOriginalType &service_object, std::tuple<FirstArgType, Args...> tuple) {
    TraitAndCall2VoidReturn(method, service_object, std::make_index_sequence<sizeof...(Args)>{}, std::move(tuple));
}

template <typename MethodType, typename ServiceOriginalType, size_t... Index, typename... Args>
inline static auto TraitAndCall2(
        const MethodType& method,
        ServiceOriginalType service_object,
        const std::index_sequence<Index ...>&, std::tuple<std::string, Args...> tuple) {
    return (service_object.*method)(std::move(std::get<Index + 1>(tuple)) ...);
}

template <typename MethodType, typename ServiceOriginalType, size_t... Index, typename... Args>
inline static auto TraitAndCall2VoidReturn(
        const MethodType& method,
        ServiceOriginalType service_object,
        const std::index_sequence<Index ...>&, std::tuple<std::string, Args...> tuple) {
    (service_object.*method)(std::move(std::get<Index + 1>(tuple)) ...);
}

}

#endif
