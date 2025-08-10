cmake_minimum_required(VERSION 3.14)
project(penaur)

include(FetchContent)

FetchContent_Declare(
        tomlplusplus
        GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
        GIT_TAG        v3.4.0
)


FetchContent_MakeAvailable(tomlplusplus)

add_executable(penaur
        src/main.cpp
        utils/config.hpp
        utils/runner.cpp
        utils/runner.hpp
)

target_link_libraries(penaur PRIVATE tomlplusplus::tomlplusplus)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
