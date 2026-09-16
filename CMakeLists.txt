cmake_minimum_required(VERSION 4.0.0)
project(spudnik CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(spudnik main.cpp)
add_library(project_warnings INTERFACE)

target_compile_options(project_warnings INTERFACE -Wall -Wextra -Werror -Wpedantic -Wunused -Wshadow -Wformat=2 -fdiagnostics-color=always -Wfatal-errors -fdiagnostics-show-caret)

target_link_libraries(spudnik PRIVATE project_warnings)




