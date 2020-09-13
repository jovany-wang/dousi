#include "gtest/gtest.h"

#include "core/dousi.h"

#include <iostream>

class TestAdder {
public:
    int add(int x, int y) { return x + y; }

    int get_sum() { return 0; }
};



TEST(Nameof, TestClassName) {
    auto remote_method = dousi::Remote(&TestAdder::add);

    ASSERT_TRUE("add" == remote_method.GetName());
    std::cout << remote_method.GetName() << std::endl;
    auto method = remote_method.GetMethod();

    TestAdder adder;

    auto s = std::invoke(method, &adder, 100 ,3000);
    ASSERT_EQ(3100, s);
}







int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
