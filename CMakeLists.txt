cmake_minimum_required(VERSION 3.28.1)

project(jvm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)

add_library(jvm)
target_sources(jvm PRIVATE
        src/jvm/Class.h
        src/jvm/Class.cc
        src/jvm/Class_Conv.cc
        src/jvm/Class_Parse.cc
)
find_package(Sese CONFIG REQUIRED)
target_link_libraries(jvm PUBLIC Sese::Core)

add_executable(runner)
target_sources(runner PRIVATE
        src/runner/Entry.cpp
)
target_link_libraries(runner PUBLIC jvm)

add_executable(test)
target_sources(test PRIVATE
        src/test/Main.cpp
        src/test/TestClass.cpp
)
target_link_libraries(test PUBLIC jvm)
find_package(GTest CONFIG REQUIRED)
target_link_libraries(test PRIVATE GTest::gtest)
