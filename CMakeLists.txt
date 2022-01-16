cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(Cubes LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Cubes)

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE CONFIGURATION="$(ConfigurationName)")