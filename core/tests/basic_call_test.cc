#include "gtest/gtest.h"

#include "test_utils/basic_services.h"

#include <nameof/nameof.hpp>
#include "core/submitter/api.h"
#include "core/dousi.h"

TEST(BasicCallTest, TestEchoer) {
    {
        // Test echo
        dousi::Init("127.0.0.1:10001");
        auto echoer = dousi::GetService("Echoer");
        auto echoed_str = echoer.Call(dousi::Remote(&Echoer::echo), "hello world").Get();
        ASSERT_TRUE("hello world" == *echoed_str);
    }
    {
        // Test calculator
        auto calculator = dousi::GetService("Calculator");
        const auto sum1 = calculator.Call(dousi::Remote(&Calculator::add), 10, 29).Get();
        ASSERT_EQ(39, *sum1);
        const auto sum2 = calculator.Call(dousi::Remote(&Calculator::add), 20, 30).Get();
        ASSERT_EQ(50, *sum2);

        const auto sub = calculator.Call(dousi::Remote(&Calculator::sub), 30, 10).Get();
        ASSERT_EQ(20, *sub);

    }
    {
        // Test void return
        auto void_return = dousi::GetService("VoidReturnService");
        const auto before = void_return.Call(dousi::Remote(&VoidReturnService::Get)).Get();
        ASSERT_TRUE("123" == *before);
        const auto performed = void_return.Call(dousi::Remote(&VoidReturnService::Perform), "456").Get();
        const auto after = void_return.Call(dousi::Remote(&VoidReturnService::Get)).Get();
        ASSERT_TRUE("456" == *after);
    }
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
