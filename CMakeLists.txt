cmake_minimum_required(VERSION 3.2)
project(MyHeaderLibrary LANGUAGES CXX)

add_library(alex_types INTERFACE)
add_library(alex_types::alex_types ALIAS alex_types)

target_include_directories(my_header_library INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(alex_types INTERFACE cxx_std_23)
