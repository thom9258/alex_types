cmake_minimum_required(VERSION 3.2)
project(alex_types LANGUAGES CXX)

add_library(alex_types INTERFACE)
add_library(alex_types::alex_types ALIAS alex_types)

target_compile_features(alex_types INTERFACE cxx_std_23)

target_include_directories(alex_types INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
