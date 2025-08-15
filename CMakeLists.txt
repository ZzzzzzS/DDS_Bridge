cmake_minimum_required(VERSION 3.16)
project(fastdds_comm)

set(CMAKE_CXX_STANDARD 17)
find_package(fastcdr REQUIRED)
find_package(fastdds REQUIRED)

option(BUILD_EXAMPLE "Build example executable" ON)

set(MSG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/msg)
set(GEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/msg/auto_gen/")
file(MAKE_DIRECTORY ${GEN_DIR})

file(GLOB_RECURSE IDL_FILES "${MSG_DIR}/*.idl")
message(STATUS "IDL files found: ${IDL_FILES}")

add_custom_target(gen_comm ALL
    COMMAND fastddsgen ${IDL_FILES} -language c++ -d ${GEN_DIR}
    COMMENT "Running fastddsgen for subscriber ..."
)

execute_process(
    COMMAND fastddsgen ${IDL_FILES} -language c++ -d ${GEN_DIR}
    RESULT_VARIABLE ret
)

# add all cxx file in msg folder
file(GLOB_RECURSE MSG_SRC "${GEN_DIR}/*.cxx")

add_library(fastdds_comm STATIC
    ${MSG_SRC}
)

add_dependencies(fastdds_comm gen_comm)

target_include_directories(fastdds_comm
    PUBLIC
    $<BUILD_INTERFACE:${GEN_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

target_link_libraries(fastdds_comm PUBLIC fastcdr fastdds)


if(BUILD_EXAMPLE)
    add_executable(pose_example example/example.cpp)
    target_link_libraries(pose_example PRIVATE fastdds_comm)
endif()
