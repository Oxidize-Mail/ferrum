cmake_minimum_required(VERSION 4.0.0)
project(spudnik CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(dpp CONFIG REQUIRED)

add_executable(spudnik main.cpp)
target_link_libraries(spudnik PRIVATE dpp::dpp)

add_library(project_warnings INTERFACE)

if (MSVC)
    target_compile_options(project_warnings INTERFACE
        /W4          # roughly equivalent to -Wall -Wextra
        /WX          # treat warnings as errors (like -Werror)
        /permissive- # stricter standards conformance
    )
else()
    target_compile_options(project_warnings INTERFACE
        -Wall -Wextra -Werror -Wpedantic -Wunused -Wshadow -Wformat=2
        -fdiagnostics-color=always -Wfatal-errors -fdiagnostics-show-caret
    )
endif()


target_link_libraries(spudnik PRIVATE project_warnings)




