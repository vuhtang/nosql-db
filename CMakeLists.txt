cmake_minimum_required(VERSION 3.21)
project(llp C)


file(GLOB_RECURSE sources CONFIGURE_DEPENDS
        src/*.c
        include/*.h
        )

add_executable(llp ${sources})
target_include_directories(llp PRIVATE src include)
