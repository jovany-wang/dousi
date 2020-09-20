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

################################## Dousi libraries ###################################
# dousi_common_lib library
ADD_LIBRARY(dousi_common_lib src/common/endpoint.cc)
ADD_DEPENDENCIES(dousi_common_lib boost_ep)

# dousi_stream_lib
ADD_LIBRARY(dousi_stream_lib src/core/stream/stream.cc)
ADD_DEPENDENCIES(dousi_stream_lib dousi_common_lib boost_ep)
TARGET_LINK_LIBRARIES(dousi_stream_lib dousi_common_lib)

# dousi_exexutor_lib
ADD_LIBRARY(dousi_executor_lib src/core/executor/executor.cc)
ADD_DEPENDENCIES(dousi_executor_lib dousi_common_lib dousi_stream_lib boost_ep)
TARGET_LINK_LIBRARIES(dousi_executor_lib dousi_common_lib dousi_stream_lib)

# dousi_submitter_lib
ADD_LIBRARY(dousi_submitter_lib src/core/submitter/submitter_runtime.cc)
ADD_DEPENDENCIES(dousi_submitter_lib dousi_common_lib dousi_stream_lib boost_ep)
TARGET_LINK_LIBRARIES(dousi_submitter_lib dousi_common_lib dousi_stream_lib)

# dousi_master_server_lib
ADD_LIBRARY(dousi_master_server_lib src/runtime/distributed/master/server/master_server.cc src/runtime/distributed/master/server/master_client_session.cc)
ADD_DEPENDENCIES(dousi_master_server_lib boost_ep dousi_common_lib gflags_ep msgpack_ep)
TARGET_LINK_LIBRARIES(dousi_master_server_lib pthread)

# dousi_master_client_lib
ADD_LIBRARY(dousi_master_client_lib src/runtime/distributed/master/client/master_client.cc)
ADD_DEPENDENCIES(dousi_master_client_lib boost_ep dousi_common_lib msgpack_ep)
TARGET_LINK_LIBRARIES(dousi_master_client_lib pthread)

################################## Dousi libraries ###################################


# master_server_main binary
ADD_EXECUTABLE(master_server_main src/runtime/distributed/master/master_server_main.cc)
ADD_DEPENDENCIES(master_server_main dousi_master_server_lib dousi_common_lib boost_ep gflags_ep msgpack_ep)
TARGET_LINK_LIBRARIES(master_server_main dousi_master_server_lib dousi_common_lib ${GFLAGS_STATIC_LIB} pthread)

# master client binary
ADD_EXECUTABLE(master_client_main src/runtime/distributed/master/master_client_main.cc)
ADD_DEPENDENCIES(master_client_main dousi_master_client_lib dousi_common_lib boost_ep msgpack_ep)
TARGET_LINK_LIBRARIES(master_client_main dousi_common_lib dousi_master_client_lib ${GFLAGS_STATIC_LIB} pthread)

# examples server binary
ADD_EXECUTABLE(example_server examples/server.cc)
ADD_DEPENDENCIES(example_server googletest_ep boost_ep dousi_common_lib dousi_executor_lib)
TARGET_LINK_LIBRARIES(example_server dousi_common_lib dousi_executor_lib pthread)

# examples client binary
ADD_EXECUTABLE(example_client examples/client.cc)
ADD_DEPENDENCIES(example_client googletest_ep boost_ep dousi_common_lib dousi_submitter_lib)
TARGET_LINK_LIBRARIES(example_client dousi_common_lib dousi_submitter_lib pthread)

# basic_server_for_test binary
ADD_EXECUTABLE(basic_server_for_test tests/test_utils/basic_server_for_test.cc)
ADD_DEPENDENCIES(basic_server_for_test googletest_ep boost_ep dousi_common_lib dousi_executor_lib)
TARGET_LINK_LIBRARIES(basic_server_for_test dousi_common_lib dousi_executor_lib pthread)

###################### Dousi Tests #######################
DEFINE_DOUSI_TEST(test_basic_call SRC_FILES tests/basic_call_test.cc LINKS dousi_common_lib dousi_submitter_lib)

DEFINE_DOUSI_TEST(test_multiple_calls SRC_FILES tests/multiple_calls_with_no_wait.cc LINKS dousi_common_lib dousi_submitter_lib)

DEFINE_DOUSI_TEST(test_master SRC_FILES src/runtime/distributed/master/master_test.cc LINKS dousi_master_server_lib dousi_common_lib dousi_master_client_lib)

DEFINE_DOUSI_TEST(test_nameof SRC_FILES tests/nameof_test.cc)

DEFINE_DOUSI_TEST(test_dousi_method_macro SRC_FILES tests/method_macro_test.cc)

###################### Dousi Tests #######################
