cmake_minimum_required(VERSION 4.0.0)
project(ferrum CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenSSL REQUIRED)
find_package(dpp CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_executable(ferrum main.cpp
        third_party/dotenv/dotenv.hpp)
add_executable(ferrum_test test/test_ferrum.cpp)

target_include_directories(ferrum PUBLIC third_party)
target_link_libraries(ferrum PRIVATE dpp::dpp)
target_link_libraries(ferrum_test PRIVATE GTest::gtest_main)
add_library(project_warnings INTERFACE)

if (MSVC)
    target_compile_options(project_warnings INTERFACE
            /W4          # roughly equivalent to -Wall -Wextra
            /WX          # treat warnings as errors (like -Werror)
            /permissive- # stricter standards conformance
    )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(project_warnings INTERFACE
            -Wall -Wextra -Werror -Wpedantic -Wunused -Wshadow -Wformat=2
            -fdiagnostics-color=always -Wfatal-errors
    )
else ()
    target_compile_options(project_warnings INTERFACE
            -Wall -Wextra -Werror -Wpedantic -Wunused -Wshadow -Wformat=2
            -fdiagnostics-color=always -Wfatal-errors
    )
endif ()


target_link_libraries(ferrum PRIVATE project_warnings)

enable_testing()
add_test(NAME test_ferrum COMMAND test_ferrum)




