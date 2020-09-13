#include "gtest/gtest.h"

#include "test_utils/basic_services.h"

#include <nameof/nameof.hpp>
#include "core/submitter/api.h"
#include "core/common/remote_annotation.h"


TEST(BasicCallTest, TestEchoer) {
    dousi::Init2("127.0.0.1:10001");
    auto echoer = dousi::GetService("Echoer");
    auto echoed_str = echoer.Call(dousi::Remote(&Echoer::echo), "hello world").Get();
    ASSERT_TRUE("hello world" == *echoed_str);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
