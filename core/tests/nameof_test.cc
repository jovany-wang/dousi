#include "gtest/gtest.h"

#include <nameof/nameof.hpp>


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

    std::cout << "--------" << NAMEOF_TYPE(void) << std::endl;
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
