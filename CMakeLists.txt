cmake_minimum_required(VERSION 3.15)

project(kage-cpp VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include_ext)

file(GLOB KAGE_CPP_SRCFILES "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB KAGETEST_SRCFILES "${PROJECT_SOURCE_DIR}/kagetest/*.cpp")

add_executable(KageCppTest ${KAGETEST_SRCFILES} ${KAGE_CPP_SRCFILES})