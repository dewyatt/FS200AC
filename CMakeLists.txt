cmake_minimum_required(VERSION 3.12)
project(FS200AC LANGUAGES CXX)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

option(BUILD_EXAMPLE FALSE)

add_library(fs200ac
    src/FS200AC.cpp
)

target_include_directories(fs200ac
    PUBLIC include
)

if (BUILD_EXAMPLE)
    find_package(serial REQUIRED)

    add_executable(fsmonitor
        example/main.cpp
        example/control_names.cpp
    )

    target_link_libraries(fsmonitor fs200ac serial)
endif()

