cmake_minimum_required(VERSION 3.28.1)

project(jvm)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(jvm)
target_sources(jvm PRIVATE src/Entry.cpp)

find_package(Sese CONFIG REQUIRED)
target_link_libraries(jvm PRIVATE Sese::Core)