CMAKE_MINIMUM_REQUIRED(VERSION 3.11)
PROJECT(dousi_core)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

FIND_PACKAGE(JNI REQUIRED)

INCLUDE(ExternalProject)
INCLUDE(CMakeUtils)
INCLUDE(GtestExternalProject)
INCLUDE(BoostExternalProject)
INCLUDE(GflagsExternalProject)
INCLUDE(MsgpackExternalProject)
INCLUDE(SpdlogExternalProject)
INCLUDE_DIRECTORIES(cpp/src)
INCLUDE_DIRECTORIES(SYSTEM ${GTEST_INCLUDE_DIR})
INCLUDE_DIRECTORIES(SYSTEM ${BOOST_INCLUDE_DIR})
INCLUDE_DIRECTORIES(SYSTEM ${GFLAGS_INCLUDE_DIR})
INCLUDE_DIRECTORIES(SYSTEM ${MSGPACK_INCLUDE_DIR})
INCLUDE_DIRECTORIES(SYSTEM ${SPDLOG_INCLUDE_DIR})
INCLUDE_DIRECTORIES(SYSTEM thirdparty)
INCLUDE_DIRECTORIES(SYSTEM ${JNI_INCLUDE_DIRS})

LINK_DIRECTORIES(${BOOST_STATIC_LIB_DIR})

################################## Dousi libraries ###################################
# dousi_common_lib library
ADD_LIBRARY(dousi_common_lib cpp/src/common/endpoint.cc cpp/src/common/logging.cc)
ADD_DEPENDENCIES(dousi_common_lib boost_ep spdlog_ep)

# dousi_stream_lib
ADD_LIBRARY(dousi_stream_lib cpp/src/core/stream/stream.cc)
ADD_DEPENDENCIES(dousi_stream_lib dousi_common_lib boost_ep)
TARGET_LINK_LIBRARIES(dousi_stream_lib dousi_common_lib)

# dousi_exexutor_lib
ADD_LIBRARY(dousi_executor_lib cpp/src/core/executor/executor.cc)
ADD_DEPENDENCIES(dousi_executor_lib dousi_common_lib dousi_stream_lib boost_ep)
TARGET_LINK_LIBRARIES(dousi_executor_lib dousi_common_lib dousi_stream_lib)

# dousi_submitter_lib
ADD_LIBRARY(dousi_submitter_lib cpp/src/core/submitter/submitter.cc)
ADD_DEPENDENCIES(dousi_submitter_lib dousi_common_lib dousi_stream_lib boost_ep)
TARGET_LINK_LIBRARIES(dousi_submitter_lib dousi_common_lib dousi_stream_lib)

# dousi_master_server_lib
ADD_LIBRARY(dousi_master_server_lib cpp/src/runtime/distributed/master/master_server.cc)
ADD_DEPENDENCIES(dousi_master_server_lib boost_ep dousi_common_lib dousi_executor_lib gflags_ep msgpack_ep)
TARGET_LINK_LIBRARIES(dousi_master_server_lib dousi_common_lib dousi_executor_lib pthread)

# dousi_master_client_lib
ADD_LIBRARY(dousi_master_client_lib cpp/src/runtime/distributed/master/master_client.cc)
ADD_DEPENDENCIES(dousi_master_client_lib dousi_submitter_lib boost_ep dousi_common_lib msgpack_ep)
TARGET_LINK_LIBRARIES(dousi_master_client_lib dousi_submitter_lib dousi_common_lib pthread)

# JNI library
ADD_LIBRARY(dousi_jni_client_lib SHARED cpp/src/jni/org_dousi_client_NativeRpcClient.cc)
ADD_DEPENDENCIES(dousi_jni_client_lib dousi_submitter_lib boost_ep dousi_common_lib msgpack_ep)
TARGET_LINK_LIBRARIES(dousi_jni_client_lib dousi_submitter_lib dousi_common_lib pthread)


# dousi default master server binary
ADD_EXECUTABLE(dousi_default_master cpp/src/runtime/distributed/master/default_master_server_main.cc)
ADD_DEPENDENCIES(dousi_default_master  dousi_master_server_lib dousi_stream_lib dousi_common_lib)
TARGET_LINK_LIBRARIES(dousi_default_master dousi_master_server_lib dousi_stream_lib dousi_common_lib pthread)

################################## Dousi libraries ###################################

# examples server binary
ADD_EXECUTABLE(example_server cpp/examples/server.cc)
ADD_DEPENDENCIES(example_server googletest_ep boost_ep dousi_common_lib dousi_executor_lib)
TARGET_LINK_LIBRARIES(example_server dousi_common_lib dousi_executor_lib pthread)

# examples client binary
ADD_EXECUTABLE(example_client cpp/examples/client.cc)
ADD_DEPENDENCIES(example_client googletest_ep boost_ep dousi_common_lib dousi_submitter_lib)
TARGET_LINK_LIBRARIES(example_client dousi_common_lib dousi_submitter_lib pthread)

# basic_server_for_test binary
ADD_EXECUTABLE(basic_server_for_test cpp/tests/test_utils/basic_server_for_test.cc)
ADD_DEPENDENCIES(basic_server_for_test googletest_ep boost_ep dousi_common_lib dousi_executor_lib)
TARGET_LINK_LIBRARIES(basic_server_for_test dousi_common_lib dousi_executor_lib pthread)

###################### Dousi Tests #######################
DEFINE_DOUSI_TEST(test_basic_call SRC_FILES cpp/tests/basic_call_test.cc LINKS dousi_common_lib dousi_submitter_lib)

DEFINE_DOUSI_TEST(test_multiple_calls SRC_FILES cpp/tests/multiple_calls_with_no_wait.cc LINKS dousi_common_lib dousi_submitter_lib)

DEFINE_DOUSI_TEST(test_master SRC_FILES cpp/src/runtime/distributed/master/master_test.cc LINKS dousi_master_server_lib dousi_master_client_lib dousi_stream_lib dousi_common_lib)

DEFINE_DOUSI_TEST(test_nameof SRC_FILES cpp/tests/nameof_test.cc)

DEFINE_DOUSI_TEST(test_dousi_method_macro SRC_FILES cpp/tests/method_macro_test.cc)

DEFINE_DOUSI_TEST(test_logging SRC_FILES cpp/tests/logging_test.cc LINKS dousi_common_lib)

DEFINE_DOUSI_TEST(test_concurrent_queue SRC_FILES cpp/tests/concurrent_queue_test.cc LINKS dousi_common_lib)

DEFINE_DOUSI_TEST(processor_main_1 SRC_FILES cpp/tests/distributed/processor1_main.cc LINKS dousi_master_server_lib dousi_master_client_lib dousi_stream_lib dousi_common_lib)
DEFINE_DOUSI_TEST(processor_main_2 SRC_FILES cpp/tests/distributed/processor2_main.cc LINKS dousi_master_server_lib dousi_master_client_lib dousi_stream_lib dousi_common_lib)
###################### Dousi Tests #######################


###################### Dousi Benchmarks #######################
ADD_EXECUTABLE(benchmark_server_single_node cpp/benchmarks/single_node/benchmark_server.cc)
ADD_DEPENDENCIES(benchmark_server_single_node dousi_common_lib dousi_executor_lib)
TARGET_LINK_LIBRARIES(benchmark_server_single_node dousi_common_lib dousi_executor_lib pthread)

ADD_EXECUTABLE(benchmark_client_single_node cpp/benchmarks/single_node/benchmark_client.cc)
ADD_DEPENDENCIES(benchmark_client_single_node dousi_common_lib dousi_submitter_lib)
TARGET_LINK_LIBRARIES(benchmark_client_single_node dousi_common_lib dousi_submitter_lib pthread)

ADD_EXECUTABLE(benchmark_mutil_thread_client cpp/benchmarks/single_node/benchmark_multi_threads_client.cc)
ADD_DEPENDENCIES(benchmark_mutil_thread_client dousi_common_lib dousi_submitter_lib)
TARGET_LINK_LIBRARIES(benchmark_mutil_thread_client dousi_common_lib dousi_submitter_lib pthread)

###################### Dousi Benchmarks #######################
