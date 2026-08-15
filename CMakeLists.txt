cmake_minimum_required(VERSION 3.20)
project(aurora)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

add_executable(aurora
        src/aurora.cpp
        include/config.h
        include/fprocess.hpp
        include/external/crow/crow_all.h
        include/restapi.hpp
        include/sysutils.hpp
)
