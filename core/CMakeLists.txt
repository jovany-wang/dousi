CMAKE_MINIMUM_REQUIRED(VERSION 3.11)
PROJECT(dousi_core)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

INCLUDE(ExternalProject)
INCLUDE(CMakeUtils)
INCLUDE(GtestExternalProject)
INCLUDE(BoostExternalProject)
INCLUDE(GflagsExternalProject)
INCLUDE(MsgpackExternalProject)
INCLUDE_DIRECTORIES(src)
INCLUDE_DIRECTORIES(SYSTEM ${GTEST_INCLUDE_DIR})
INCLUDE_DIRECTORIES(SYSTEM ${BOOST_INCLUDE_DIR})
INCLUDE_DIRECTORIES(SYSTEM ${GFLAGS_INCLUDE_DIR})
INCLUDE_DIRECTORIES(SYSTEM ${MSGPACK_INCLUDE_DIR})
INCLUDE_DIRECTORIES(SYSTEM thirdparty)

LINK_DIRECTORIES(${BOOST_STATIC_LIB_DIR})

ADD_LIBRARY(dousi_common_lib src/common/endpoint.cc)
ADD_DEPENDENCIES(dousi_common_lib boost_ep)

# master_server_static_library
ADD_LIBRARY(master_server src/master/server/master_server.cc src/master/server/master_client_session.cc)
ADD_DEPENDENCIES(master_server boost_ep dousi_common_lib gflags_ep msgpack_ep)
TARGET_LINK_LIBRARIES(master_server pthread)

# master_client_static_library
ADD_LIBRARY(master_client src/master/client/master_client.cc)
ADD_DEPENDENCIES(master_client boost_ep dousi_common_lib msgpack_ep)
TARGET_LINK_LIBRARIES(master_client pthread)

# master_server_main binary
ADD_EXECUTABLE(master_server_main src/master/master_server_main.cc)
ADD_DEPENDENCIES(master_server_main master_server dousi_common_lib boost_ep gflags_ep msgpack_ep)
TARGET_LINK_LIBRARIES(master_server_main master_server dousi_common_lib ${GFLAGS_STATIC_LIB} pthread)

# master client binary
ADD_EXECUTABLE(master_client_main src/master/master_client_main.cc)
ADD_DEPENDENCIES(master_client_main master_client dousi_common_lib boost_ep msgpack_ep)
TARGET_LINK_LIBRARIES(master_client_main dousi_common_lib master_client ${GFLAGS_STATIC_LIB} pthread)

# examples server
ADD_EXECUTABLE(example_server
        examples/server.cc src/core/submitter/submitter_runtime.cc
        src/core/executor/connection_session.cc
        src/core/executor/executor_runtime.cc )
ADD_DEPENDENCIES(example_server googletest_ep boost_ep dousi_common_lib)
TARGET_LINK_LIBRARIES(example_server dousi_common_lib pthread)

# examples client
ADD_EXECUTABLE(example_client
        examples/client.cc src/core/submitter/submitter_runtime.cc)
ADD_DEPENDENCIES(example_client googletest_ep boost_ep dousi_common_lib)
TARGET_LINK_LIBRARIES(example_client dousi_common_lib pthread)

# basic_server_for_test
ADD_EXECUTABLE(basic_server_for_test
        tests/test_utils/basic_server_for_test.cc
        src/core/executor/connection_session.cc
        src/core/executor/executor_runtime.cc)
ADD_DEPENDENCIES(basic_server_for_test googletest_ep boost_ep dousi_common_lib)
TARGET_LINK_LIBRARIES(basic_server_for_test dousi_common_lib pthread)

DEFINE_DOUSI_TEST(test_basic_call
        SRC_FILES tests/basic_call_test.cc src/core/submitter/submitter_runtime.cc
        LINKS dousi_common_lib)

DEFINE_DOUSI_TEST(test_master
        SRC_FILES src/master/master_test.cc
        LINKS master_server dousi_common_lib master_client)

DEFINE_DOUSI_TEST(test_nameof
        SRC_FILES tests/nameof_test.cc
        LINKS dousi_common_lib)

DEFINE_DOUSI_TEST(test_dousi_method_macro
        SRC_FILES tests/method_macro_test.cc src/core/executor/executor_runtime.cc
        LINKS dousi_common_lib)
