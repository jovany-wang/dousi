#include "gtest/gtest.h"

#include <nameof/nameof.hpp>

#include <iostream>

class MyClass {
public:
    void VoidMyMethod0() {}

    void VoidMyMethod1(int a) {}

    void VoidMyMethod2() {}

    void VoidMyMethod2(int a) {}
};



TEST(Nameof, TestClassName) {
    constexpr auto class_name = NAMEOF_TYPE(MyClass);
    ASSERT_EQ("MyClass", class_name);
}

TEST(Nameof, TestMethodName) {
    constexpr auto void_my_method0 = NAMEOF(&MyClass::VoidMyMethod0);
    ASSERT_EQ("VoidMyMethod0", void_my_method0);

    constexpr auto void_my_method1 = NAMEOF(&MyClass::VoidMyMethod1);
    ASSERT_EQ("VoidMyMethod1", void_my_method1);
}


#define MY_NAMEOF(T) NAMEOF(T)

template <typename ClassType>
using VoidMethodType = void (ClassType::*) ();

template <typename ClassType>
inline std::string GetMethodName(VoidMethodType<ClassType> method) {
    constexpr auto method_name = MY_NAMEOF(method);
    return std::string {method_name};
}

template <typename ClassType>
inline void RegisterService(const std::string &name, VoidMethodType<ClassType> method) {

}

#define REGISTER_SERVICE(METHOD) MY_NAMEOF(METHOD)

TEST(Nameof, TestNethodNamePassedByArgs) {
    MyClass c;
    std::cout << MY_NAMEOF(c.VoidMyMethod0()) << std::endl;
    const auto void_my_method_0 = GetMethodName(&MyClass::VoidMyMethod0);
    std::cout << void_my_method_0 << std::endl;
    std::cout << REGISTER_SERVICE(&MyClass::VoidMyMethod0);
}

class Runnable {
protected:
        virtual void run() = 0;
//        static Runnable &make();
};

class MyTaskA : public Runnable {
public:
    void run() override {
        //
    }
    // 算make在MyTaskA中的偏移量, 使用动态加载库方法
    static std::shared_ptr<Runnable> make() {
        return std::make_shared<MyTaskA>();
    }
};

template <typename TaskType>
int AsyncTask() {
    std::cout << NAMEOF_TYPE(TaskType) << std::endl;
    return 1;
}


void ignore(int a) {

}

TEST(Nameof, TestAsyncTask) {
//    这里需要解决的问题就是MyTaskA对象 如何在别的节点上创建成功？
//    哎 没有反射好像真的不太行，要不在Java上实现这个算了，C++还是老老实实老样子吧。
    auto task = AsyncTask<MyTaskA>();
    ignore(task);
//    Dousi::Submit(task);
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
