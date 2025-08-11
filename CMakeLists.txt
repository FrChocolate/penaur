cmake_minimum_required(VERSION 3.14)
project(penaur)


add_executable(penaur
        src/main.cpp
        utils/runner.hpp
        utils/config.hpp
        utils/runner.hpp
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
