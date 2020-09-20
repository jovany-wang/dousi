#include "gtest/gtest.h"

#include "test_utils/basic_services.h"

#include <nameof/nameof.hpp>
#include "core/submitter/api.h"
#include "core/dousi.h"

TEST(MultipleCallsTest, TestCallsWithNoWait) {
    {
        /// This test tests that we repeatedly calls multiple times with no any `Get()`.
        /// It can test if the client send the some parts of one message together.
        dousi::Init("127.0.0.1:10002");
        auto echoer = dousi::GetService("Echoer");
        echoer.Call(dousi::Remote(&Echoer::echo), "A");
        echoer.Call(dousi::Remote(&Echoer::echo), "B");
        auto echoed_str = echoer.Call(dousi::Remote(&Echoer::echo), "C").Get();
        ASSERT_TRUE("C" == *echoed_str);
    }

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
