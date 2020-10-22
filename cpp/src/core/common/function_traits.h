#include <iostream>
#include <string>
#include <tuple>
#include <functional>

template <typename R, typename... Args>
struct BaseFunctionTraits {
public:
    static constexpr auto ArgsNum = sizeof...(Args);

    using ReturnType = R;

    template <std::size_t N>
    using ArgsType = std::tuple_element_t<N, std::tuple<Args...>>;

    using MethodNameWithArgsTuple = std::tuple<std::string, std::remove_const_t<std::remove_reference_t<Args>>...>;
};

template <typename T>
struct FunctionTraits;

/// The traits for a normal function like:
///      void (int);
template <typename R, typename... Args>
struct FunctionTraits<R(Args...)> : public BaseFunctionTraits<R, Args...> {};

/// The traits for a function pointer like:
///         void(*) (int, int);
template <typename R, typename... Args>
struct FunctionTraits<R(*)(Args...)> : public BaseFunctionTraits<R, Args...> {};

/// The traits for a function reference like:
///        void(&) (int, int);
template <typename R, typename... Args>
struct FunctionTraits<R(&)(Args...)> : public BaseFunctionTraits<R, Args...> {};

/// The traits for a const function like:
///        void(&) (int, int) const;
template <typename R, typename... Args>
struct FunctionTraits<R(Args...) const> : public BaseFunctionTraits<R, Args...> {};

/// The traits for a lambda or a std::function like:
///        main()::<lambda(int, int)>;
//         std::function<int(int, int)>;
template <typename T>
struct FunctionTraits : public FunctionTraits<decltype(&T::operator())> {};


/// The traits for a lambda pointer or a std::function pointer like:
///       int(main()::<lambda(int, int)>*)(int, int);
///       int(std::function<int(int, int)>)(int, int);
template <typename ClassType, typename R, typename... Args>
struct FunctionTraits<R(ClassType::*)(Args...)> : public BaseFunctionTraits<R, Args...> {};

/// The traits for a const lambda or a const std::function like:
///       int(main()::<lambda(int, int)>*)(int, int) const;
///       int(std::function<int(int, int)>)(int, int) const;
template <typename ClassType, typename R, typename... Args>
struct FunctionTraits<R(ClassType::*)(Args...) const> : public BaseFunctionTraits<R, Args...> {};


/////// Another trait to trait if it's a void return method.
struct VoidReturnTag {};
struct NonVoidReturnTag {};

template <typename ReturnType>
struct VoidReturnTrait {
    using Tag = NonVoidReturnTag;
};

template <>
struct VoidReturnTrait<void> {
    using Tag = VoidReturnTag;
};
