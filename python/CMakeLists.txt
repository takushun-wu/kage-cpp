cmake_minimum_required(VERSION 3.15)

project(kage-cpp-python LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(KAGE_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
file(GLOB KAGE_CORE_SRCFILES "${KAGE_ROOT}/src/*.cpp")

set(PYBIND11_FINDPYTHON ON)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(kage_cpp MODULE
    kage_engine.cpp
    ${KAGE_CORE_SRCFILES}
)
target_include_directories(kage_cpp PRIVATE
    ${KAGE_ROOT}/include
    ${KAGE_ROOT}/include_ext
)
set_target_properties(kage_cpp PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/python"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/python"
)