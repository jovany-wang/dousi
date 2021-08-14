CMAKE_MINIMUM_REQUIRED(VERSION 3.11)
PROJECT(dousi_rpc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

INCLUDE(ExternalProject)
INCLUDE(CMakeUtils)
INCLUDE(GtestExternalProject)
INCLUDE(BoostExternalProject)
INCLUDE(GflagsExternalProject)
INCLUDE(MsgpackExternalProject)
INCLUDE(SpdlogExternalProject)
INCLUDE_DIRECTORIES(src/dousi)
INCLUDE_DIRECTORIES(SYSTEM ${GTEST_INCLUDE_DIR})
INCLUDE_DIRECTORIES(SYSTEM ${BOOST_INCLUDE_DIR})
INCLUDE_DIRECTORIES(SYSTEM ${GFLAGS_INCLUDE_DIR})
INCLUDE_DIRECTORIES(SYSTEM ${MSGPACK_INCLUDE_DIR})
INCLUDE_DIRECTORIES(SYSTEM ${SPDLOG_INCLUDE_DIR})
INCLUDE_DIRECTORIES(SYSTEM src/thirdparty)

LINK_DIRECTORIES(${BOOST_STATIC_LIB_DIR})

################################## Dousi libraries ###################################
# dousi_common_lib library
ADD_LIBRARY(dousi_common_lib src/dousi/common/endpoint.cc src/dousi/common/logging.cc)
ADD_DEPENDENCIES(dousi_common_lib boost_ep spdlog_ep msgpack_ep)

# dousi_stream_lib
ADD_LIBRARY(dousi_stream_lib src/dousi/core/stream/stream.cc)
ADD_DEPENDENCIES(dousi_stream_lib dousi_common_lib boost_ep)
TARGET_LINK_LIBRARIES(dousi_stream_lib dousi_common_lib)

# dousi_exexutor_lib
ADD_LIBRARY(dousi_executor_lib src/dousi/core/executor/executor.cc)
ADD_DEPENDENCIES(dousi_executor_lib dousi_common_lib dousi_stream_lib boost_ep)
TARGET_LINK_LIBRARIES(dousi_executor_lib dousi_common_lib dousi_stream_lib)

# dousi_submitter_lib
ADD_LIBRARY(dousi_submitter_lib src/dousi/core/submitter/submitter.cc)
ADD_DEPENDENCIES(dousi_submitter_lib dousi_common_lib dousi_stream_lib boost_ep)
TARGET_LINK_LIBRARIES(dousi_submitter_lib dousi_common_lib dousi_stream_lib)

################################## Dousi libraries ###################################

# examples server binary
ADD_EXECUTABLE(example_server src/examples/server.cc)
ADD_DEPENDENCIES(example_server googletest_ep boost_ep dousi_common_lib dousi_executor_lib)
TARGET_LINK_LIBRARIES(example_server dousi_common_lib dousi_executor_lib pthread)

# examples client binary
ADD_EXECUTABLE(example_client src/examples/client.cc)
ADD_DEPENDENCIES(example_client googletest_ep boost_ep dousi_common_lib dousi_submitter_lib)
TARGET_LINK_LIBRARIES(example_client dousi_common_lib dousi_submitter_lib pthread)

# basic_server_for_test binary
ADD_EXECUTABLE(basic_server_for_test src/tests/test_utils/basic_server_for_test.cc)
ADD_DEPENDENCIES(basic_server_for_test googletest_ep boost_ep dousi_common_lib dousi_executor_lib)
TARGET_LINK_LIBRARIES(basic_server_for_test dousi_common_lib dousi_executor_lib pthread)

###################### Dousi Tests #######################
DEFINE_DOUSI_TEST(test_basic_call SRC_FILES src/tests/basic_call_test.cc LINKS dousi_common_lib dousi_submitter_lib)

DEFINE_DOUSI_TEST(test_multiple_calls SRC_FILES src/tests/multiple_calls_with_no_wait.cc LINKS dousi_common_lib dousi_submitter_lib)

DEFINE_DOUSI_TEST(test_nameof SRC_FILES src/tests/nameof_test.cc)

DEFINE_DOUSI_TEST(test_dousi_method_macro SRC_FILES src/tests/method_macro_test.cc)

DEFINE_DOUSI_TEST(test_logging SRC_FILES src/tests/logging_test.cc LINKS dousi_common_lib)

DEFINE_DOUSI_TEST(test_concurrent_queue SRC_FILES src/tests/concurrent_queue_test.cc LINKS dousi_common_lib)

###################### Dousi Tests #######################


###################### Dousi Benchmarks #######################
ADD_EXECUTABLE(benchmark_server_single_node src/benchmarks/single_node/benchmark_server.cc)
ADD_DEPENDENCIES(benchmark_server_single_node dousi_common_lib dousi_executor_lib)
TARGET_LINK_LIBRARIES(benchmark_server_single_node dousi_common_lib dousi_executor_lib pthread)

ADD_EXECUTABLE(benchmark_client_single_node src/benchmarks/single_node/benchmark_client.cc)
ADD_DEPENDENCIES(benchmark_client_single_node dousi_common_lib dousi_submitter_lib)
TARGET_LINK_LIBRARIES(benchmark_client_single_node dousi_common_lib dousi_submitter_lib pthread)

ADD_EXECUTABLE(benchmark_mutil_thread_client src/benchmarks/single_node/benchmark_multi_threads_client.cc)
ADD_DEPENDENCIES(benchmark_mutil_thread_client dousi_common_lib dousi_submitter_lib)
TARGET_LINK_LIBRARIES(benchmark_mutil_thread_client dousi_common_lib dousi_submitter_lib pthread)

###################### Dousi Benchmarks #######################
