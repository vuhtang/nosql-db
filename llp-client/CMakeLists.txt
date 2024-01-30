cmake_minimum_required(VERSION 3.21)
project(llp_client C)


file(GLOB_RECURSE sources CONFIGURE_DEPENDS
        src/*.c
        include/*.h
        )

add_executable(llp_client ${sources})
target_compile_options(llp_client PUBLIC -Wno-error=implicit-function-declaration
        )

find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

target_include_directories(llp_client PRIVATE src include)

target_link_libraries(llp_client ${LIBXML2_LIBRARIES})