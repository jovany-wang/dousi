#include "gtest/gtest.h"

#include "test_utils/basic_services.h"

#include <nameof/nameof.hpp>
#include "core/dousi.h"
#include "rpc/rpc_client.h"

TEST(BasicCallTest, TestEchoer) {
    dousi::RpcClient rpc_client {"127.0.0.1:10002"};
    {
        // Test echo
        auto echoer = rpc_client.GetService("Echoer");
        auto echoed_str = echoer.Call(dousi::Remote(&Echoer::echo), "hello world").Get();
        ASSERT_TRUE("hello world" == *echoed_str);
    }
    {
        // Test calculator
        auto calculator = rpc_client.GetService("Calculator");
        const auto sum1 = calculator.Call(dousi::Remote(&Calculator::add), 10, 29).Get();
        ASSERT_EQ(39, *sum1);
        const auto sum2 = calculator.Call(dousi::Remote(&Calculator::add), 20, 30).Get();
        ASSERT_EQ(50, *sum2);

        const auto sub = calculator.Call(dousi::Remote(&Calculator::sub), 30, 10).Get();
        ASSERT_EQ(20, *sub);
    }
    {
        // Test void return
        auto void_return = rpc_client.GetService("VoidReturnService");
        const auto before = void_return.Call(dousi::Remote(&VoidReturnService::Get)).Get();
        ASSERT_TRUE("123" == *before);
        void_return.Call(dousi::Remote(&VoidReturnService::Perform), "456");
        const auto after = void_return.Call(dousi::Remote(&VoidReturnService::Get)).Get();
        ASSERT_TRUE("456" == *after);
    }

    {
        // Test user-defined class as argument or return type.
        auto user_defined = rpc_client.GetService("UserDefinedClass");
        Person p {"Allen", 19};
        const auto person = user_defined.Call(dousi::Remote(&UserDefinedClass::IncrAge), p, 2);
        ASSERT_TRUE("Allen" == person.Get()->name);
        ASSERT_EQ(21, person.Get()->age);
    }
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
