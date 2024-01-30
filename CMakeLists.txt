cmake_minimum_required(VERSION 3.21)
project(llp C)


file(GLOB_RECURSE sources CONFIGURE_DEPENDS
        src/*.c
        include/*.h
        )

find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

add_executable(llp ${sources})
target_include_directories(llp PRIVATE src include)

target_link_libraries(llp ${LIBXML2_LIBRARIES})
