cmake_minimum_required(VERSION 4.0.0)
project(ferrum CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenSSL REQUIRED)
find_package(dpp CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_library(project_warnings INTERFACE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

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

add_executable(ferrum main.cpp
        third_party/dotenv/dotenv.hpp)
add_executable(test_ferrum test/test_ferrum.cpp)

target_include_directories(ferrum PUBLIC third_party)
target_include_directories(test_ferrum SYSTEM PUBLIC third_party)
target_link_libraries(ferrum PRIVATE 
    dpp::dpp 
    project_warnings)
target_link_libraries(test_ferrum PRIVATE 
    GTest::gtest_main 
    project_warnings)


target_link_libraries(ferrum PRIVATE project_warnings)

enable_testing()
add_test(NAME test_ferrum COMMAND test_ferrum)




