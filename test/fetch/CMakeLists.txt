cmake_minimum_required(VERSION 3.2)

project(test_fetch VERSION 0.1.0)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -ggdb -pg")
set(CMAKE_CXX_STANDARD 23)
set(CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    alex_types
    GIT_REPOSITORY https://github.com/thom9258/alex_types
    GIT_TAG        main
)

FetchContent_MakeAvailable(alex_types)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} 
  PRIVATE
  alex_types::alex_types
)
