cmake_minimum_required(VERSION 3.14)
project(penaur)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(penaur
        src/main.cpp
        utils/runner.hpp
        utils/config.hpp
        utils/colors.hpp
        utils/logger.hpp

)


find_package(PkgConfig REQUIRED)
pkg_check_modules(SECcomp REQUIRED libseccomp)


target_include_directories(penaur PRIVATE ${SECcomp_INCLUDE_DIRS})
target_link_libraries(penaur PRIVATE ${SECcomp_LIBRARIES})
target_link_directories(penaur PRIVATE ${SECcomp_LIBRARY_DIRS})
