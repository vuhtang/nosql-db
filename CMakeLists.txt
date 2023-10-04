cmake_minimum_required(VERSION 3.23)
project(llp C)

set(CMAKE_C_STANDARD 99)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS
        src/*.c
        include/*.h
        )

add_executable(llp ${sources})
target_include_directories(llp PRIVATE src include)
