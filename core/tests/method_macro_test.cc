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

    std::cout << remote_method.GetName() << std::endl;
    auto method = remote_method.GetMethod();

    TestAdder adder;

    auto s = std::invoke(method, &adder, 100 ,3000);
    std::cout << "s = " << s << std::endl;

}







int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
