cmake_minimum_required(VERSION 3.20)
project(justore)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

add_executable(justore
        src/justore.cpp
        include/config.h
        include/fprocess.hpp
        include/external/crow/crow_all.h
)
