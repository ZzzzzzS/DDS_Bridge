cmake_minimum_required(VERSION 3.16)
project(DDS_Bridge)

set(CMAKE_CXX_STANDARD 17)
find_package(fastcdr REQUIRED)
find_package(fastdds REQUIRED)

option(BUILD_EXAMPLE "Build example executable" OFF)
option(BUILD_DOC "Build documentation" OFF)

set(MSG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/msg)
set(GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/msg/auto_gen/")
file(MAKE_DIRECTORY ${GEN_DIR})

file(GLOB IDL_FILES "${MSG_DIR}/*.idl")
message(STATUS "IDL files found: ${IDL_FILES}")

# add_custom_target(gen_comm ALL
#     COMMAND fastddsgen ${IDL_FILES} -language c++ -d ${GEN_DIR} -replace -cs -I ${MSG_DIR} -typeros2
#     COMMENT "Running fastddsgen for subscriber ..."
# )

if(WIN32)
    execute_process(
        COMMAND cmd /c fastddsgen ${IDL_FILES} -language c++ -d ${GEN_DIR} -replace -cs -I ${MSG_DIR} -typeros2
        RESULT_VARIABLE ret
        OUTPUT_VARIABLE out
        ERROR_VARIABLE err
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
    )
else()
    execute_process(
        COMMAND fastddsgen ${IDL_FILES} -language c++ -d ${GEN_DIR} -replace -cs -I ${MSG_DIR} -typeros2
        RESULT_VARIABLE ret
        OUTPUT_VARIABLE out
        ERROR_VARIABLE err
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
    )
endif()
message(STATUS "fastddsgen ret: ${ret}")
message(STATUS "fastddsgen out: ${out}")
message(STATUS "fastddsgen err: ${err}")

# add all cxx file in msg folder
file(GLOB_RECURSE MSG_SRC "${GEN_DIR}/*.cxx")

add_library(DDS_Bridge STATIC
    ${MSG_SRC}
)

#add_dependencies(DDS_Bridge gen_comm)

target_include_directories(DDS_Bridge
    PUBLIC
    $<BUILD_INTERFACE:${GEN_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

target_link_libraries(DDS_Bridge PUBLIC fastcdr fastdds)


if(BUILD_EXAMPLE)
    add_executable(pose_example example/example.cpp)
    target_link_libraries(pose_example PRIVATE DDS_Bridge)
    add_executable(ros2_comm_example example/comm_ros2.cpp)
    target_link_libraries(ros2_comm_example PRIVATE DDS_Bridge)
endif()

if(BUILD_DOC)
    add_subdirectory(doc)
endif(BUILD_DOC)
