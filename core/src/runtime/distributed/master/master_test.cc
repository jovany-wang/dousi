#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "master_server.h"
#include "master_client.h"
#include "common/endpoint.h"
#include "common/logging.h"

#include <boost/asio.hpp>

#include <thread>
#include <iostream>
#include <chrono>


using namespace dousi;
using namespace dousi::master;


/**
 * This BasicTest tests that the basic functionality between master server
 * and master client works fine.
 */
TEST(MasterTest, BasicTest) {
    std::thread backend_thread {[]() {
        MasterServer master_server("0.0.0.0:10002");
    }};

    std::this_thread::sleep_for(std::chrono::milliseconds {1 * 1000});
    MasterClient master_client1("127.0.0.1:10002");
    MasterClient master_client2("127.0.0.1:10002");
    MasterClient master_client3("127.0.0.1:10002");

  master_client1.RegisterService("adder1", "127.0.0.1:10001");
  master_client2.RegisterService("adder2", "127.0.0.1:10002");
  master_client3.RegisterService("adder3", "127.0.0.1:10003");
  master_client1.RegisterService("adder4", "127.0.0.1:10004");
  master_client1.RegisterService("adder5", "127.0.0.1:10005");
  master_client2.RegisterService("adder3", "0.0.0.0:0000");

  auto endpoints = master_client1.GetAllEndpoints();

  ASSERT_EQ(5, endpoints.size());
  ASSERT_EQ("127.0.0.1:10001", endpoints["adder1"]);
  ASSERT_EQ("127.0.0.1:10002", endpoints["adder2"]);
  ASSERT_EQ("0.0.0.0:0000", endpoints["adder3"]);
  ASSERT_EQ("127.0.0.1:10004", endpoints["adder4"]);
  ASSERT_EQ("127.0.0.1:10005", endpoints["adder5"]);

  backend_thread.detach();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
