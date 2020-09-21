#include "common/logging.h"
#include "gtest/gtest.h"
#include <fstream>

TEST(LoggingTest, Testlog) {

    dousi::DousiLog::StartDousiLog("log/test.log", dousi::LOG_LEVEL::INFO, 10, 3);
    DOUSI_LOG(DEBUG) << "this debug message won't show up " << 456;
    DOUSI_LOG(WARNING) << "Hello " << 123;
    DOUSI_LOG(INFO) << "world " << 456 << " 789";
    DOUSI_CHECK(true) << "This is a RAFTCPP_CHECK"
                      << " message but it won't show up";
    dousi::DousiLog::ShutDownDousiLog();
    std::fstream file("log/test.log");
    ASSERT_TRUE(file);

    std::string line;
    uint8_t debug_count = 0;
    uint8_t info_count = 0;
    uint8_t warn_count = 0;
    while (getline(file, line)) {
        std::cout << "line:" << line << std::endl;
        if (line.find("debug") != std::string::npos) {
            debug_count++;
        } else if (line.find("info") != std::string::npos) {
            info_count++;
        } else if (line.find("warning") != std::string::npos) {
            warn_count++;
        }
    }

    ASSERT_EQ(debug_count, 0);
    ASSERT_EQ(info_count, 1);
    ASSERT_EQ(warn_count, 1);
    file.close();
    std::remove("log/test.log");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
