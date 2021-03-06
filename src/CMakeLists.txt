set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Import glog
FetchContent_Declare(
  glog
  GIT_REPOSITORY https://github.com/google/glog.git
  GIT_TAG v0.4.0
)
FetchContent_MakeAvailable(glog)

# Import gflags
FetchContent_Declare(
  gflags
  GIT_REPOSITORY https://github.com/gflags/gflags.git
  GIT_TAG v2.2.2
)
FetchContent_MakeAvailable(gflags)

# Import gtest
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.10.0
)
FetchContent_MakeAvailable(googletest)

# Protobuf
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")

# Protobuf-compiler
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

# gRPC
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")
set(_GRPC_GRPCPP gRPC::grpc++)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

# KVStore Proto file
get_filename_component(kvstore_proto "proto/keyvaluestore.proto" ABSOLUTE)
get_filename_component(kvstore_proto_path "${kvstore_proto}" PATH)

get_filename_component(storage_proto "proto/storage.proto" ABSOLUTE)
get_filename_component(storage_proto_path "${storage_proto}" PATH)

# Generated sources
set(kvstore_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/keyvaluestore.pb.cc")
set(kvstore_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/keyvaluestore.pb.h")
set(kvstore_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/keyvaluestore.grpc.pb.cc")
set(kvstore_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/keyvaluestore.grpc.pb.h")

set(storage_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/storage.pb.cc")
set(storage_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/storage.pb.h")
set(storage_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/storage.grpc.pb.cc")
set(storage_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/storage.grpc.pb.h")

message(STATUS "current cmake dir = ${CMAKE_CURRENT_BINARY_DIR}")

message(STATUS "kvstore path = ${kvstore_proto_path}")

# aux_source_directory(. DIR_LIB_SRCS)

# add_library(kvstore_lib STATIC "keyvaluestoreclass.cc")

add_custom_command(
      OUTPUT "${kvstore_proto_srcs}" "${kvstore_proto_hdrs}" "${kvstore_grpc_srcs}" "${kvstore_grpc_hdrs}" 
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${kvstore_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${kvstore_proto}"
      DEPENDS "${kvstore_proto}")

add_custom_command(
  OUTPUT "${storage_proto_srcs}" "${storage_proto_hdrs}" "${storage_grpc_srcs}" "${storage_grpc_hdrs}" 
  COMMAND ${_PROTOBUF_PROTOC}
  ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
    -I "${storage_proto_path}"
    --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    "${storage_proto}"
  DEPENDS "${storage_proto}")

# Caw Proto file
get_filename_component(caw_proto "proto/caw.proto" ABSOLUTE)
get_filename_component(caw_proto_path "${caw_proto}" PATH)

# Generated sources
set(caw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/caw.pb.cc")
set(caw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/caw.pb.h")
set(caw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/caw.grpc.pb.cc")
set(caw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/caw.grpc.pb.h")

add_custom_command(
      OUTPUT "${caw_proto_srcs}" "${caw_proto_hdrs}" "${caw_grpc_srcs}" "${caw_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${caw_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${caw_proto}"
      DEPENDS "${caw_proto}")

# Caw Proto file
get_filename_component(faz_proto "proto/faz.proto" ABSOLUTE)
get_filename_component(faz_proto_path "${faz_proto}" PATH)

# Generated sources
set(faz_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/faz.pb.cc")
set(faz_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/faz.pb.h")
set(faz_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/faz.grpc.pb.cc")
set(faz_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/faz.grpc.pb.h")

add_custom_command(
      OUTPUT "${faz_proto_srcs}" "${faz_proto_hdrs}" "${faz_grpc_srcs}" "${faz_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${faz_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${faz_proto}"
      DEPENDS "${faz_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Targets (client|server)
foreach(_target
   kvstore_server)
  add_executable(${_target} "${_target}.cc"
    ${kvstore_proto_srcs}
    ${kvstore_grpc_srcs}
    ${storage_proto_srcs}
    ${storage_grpc_srcs}
    "keyvaluestoreclass.cc"
    )
  target_link_libraries(${_target}
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
    gflags)
endforeach()

# test kvstore
add_executable(kvstore_test "kvstore_test.cc" "keyvaluestoreclass.cc" ${storage_grpc_srcs} ${storage_proto_srcs})

target_link_libraries(kvstore_test gtest ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

# test kvstore server
add_executable(kvstore_server_test "kvstore_server_test.cc" ${kvstore_proto_srcs} 
${kvstore_grpc_srcs} "kvstore_client.cc" ${caw_proto_srcs} ${caw_grpc_srcs}
${faz_proto_srcs} ${faz_grpc_srcs})

target_link_libraries(kvstore_server_test gtest glog
${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

add_executable(kvstore_server_load_test "kvstore_server_load_test.cc" ${kvstore_proto_srcs} 
${kvstore_grpc_srcs} "kvstore_client.cc" ${caw_proto_srcs} ${caw_grpc_srcs}
${faz_proto_srcs} ${faz_grpc_srcs})

target_link_libraries(kvstore_server_load_test gtest glog
${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

# test caw functionality

add_executable(caw_test "caw_test.cc" ${caw_proto_srcs} ${caw_grpc_srcs} "caw.cc" 
${kvstore_proto_srcs} ${kvstore_grpc_srcs} "keyvaluestoreclass.cc" "kvstore_client.cc"
${faz_proto_srcs} ${faz_grpc_srcs} ${storage_grpc_srcs} ${storage_proto_srcs})

target_link_libraries(caw_test gtest glog
${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

# build faz

add_executable(faz_server "faz_server.cc" ${faz_proto_srcs} ${faz_grpc_srcs} 
"kvstore_client.cc" ${kvstore_proto_srcs} ${kvstore_grpc_srcs} "caw.cc" ${caw_proto_srcs} ${caw_grpc_srcs})

target_link_libraries(faz_server ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF} gtest glog)

# build cmd client
add_executable(client "run_cmd_client.cc" "cmd_client.cc" "faz_client.cc" ${faz_proto_srcs} ${faz_grpc_srcs} 
"kvstore_client.cc" ${kvstore_proto_srcs} ${kvstore_grpc_srcs} "caw.cc" ${caw_proto_srcs} ${caw_grpc_srcs})

target_link_libraries(client ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF} gtest glog gflags)

# test util
add_executable(util_test "util_test.cc")

target_link_libraries(util_test gtest glog)
