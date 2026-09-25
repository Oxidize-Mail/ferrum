cmake_minimum_required(VERSION 4.0.0)
project(ferrum CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

find_package(OpenSSL REQUIRED)
find_package(dpp CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_library(project_warnings INTERFACE)
# GCC accepts -Wunreachable-code but ignores it; Clang enforces it.
target_compile_options(project_warnings INTERFACE
        -Wall -Wextra -Werror -Wpedantic -Wunused -Wshadow -Wformat=2
        -fdiagnostics-color=always -Wfatal-errors -Wunreachable-code
)

# config compiler setup
add_library(ferrum_config STATIC src/config.cpp src/commands.cpp)
target_include_directories(ferrum_config PUBLIC include third_party)
target_link_libraries(ferrum_config PUBLIC dpp::dpp)
target_link_libraries(ferrum_config PRIVATE project_warnings)

# main executable setup
add_executable(ferrum main.cpp)
target_include_directories(ferrum SYSTEM PRIVATE third_party)
target_link_libraries(ferrum PRIVATE
        dpp::dpp
        project_warnings
        ferrum_config
)


# test executable setup
add_executable(test_ferrum
        test/test_ferrum.cpp
        test/test_config.cpp
)
target_include_directories(test_ferrum PRIVATE include)
target_include_directories(test_ferrum SYSTEM PUBLIC third_party)
target_link_libraries(test_ferrum PRIVATE
        GTest::gtest_main
        project_warnings
        ferrum_config
)
enable_testing()
add_test(NAME test_ferrum COMMAND test_ferrum)
