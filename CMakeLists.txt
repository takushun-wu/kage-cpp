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

option(KAGE_BUILD_PYTHON "Build the pybind11 Python module" OFF)

if(KAGE_BUILD_PYTHON)
    set(PYBIND11_FINDPYTHON ON)
    find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module)
    find_package(pybind11 CONFIG REQUIRED)

    pybind11_add_module(kage_cpp MODULE
        python/kage_engine.cpp
        ${KAGE_CPP_SRCFILES}
    )
    target_include_directories(kage_cpp PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include_ext
    )
    set_target_properties(kage_cpp PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/python"
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/python"
    )

    add_custom_target(KagePython
        DEPENDS kage_cpp
        COMMENT "Build the kage_cpp Python module"
    )
endif()
