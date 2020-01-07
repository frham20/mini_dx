cmake_minimum_required(VERSION 3.16)
project(mini_dx VERSION 0.1 LANGUAGES CXX)

include(GNUInstallDirs)

add_executable(mini_dx WIN32)

target_include_directories(mini_dx PRIVATE "src")
target_compile_definitions(mini_dx PRIVATE UNICODE)
target_compile_options(mini_dx PRIVATE /Wall /WX)

add_subdirectory(src)
