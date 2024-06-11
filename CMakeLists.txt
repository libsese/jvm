cmake_minimum_required(VERSION 3.13)

project(jvm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/utf-8)
    if (VCPKG_TARGET_TRIPLET STREQUAL "x64-windows-static")
        set(CMAKE_CXX_FLAGS_DEBUG /MTd)
        set(CMAKE_CXX_FLAGS_MINSIZEREL /MT)
        set(CMAKE_CXX_FLAGS_RELEASE /MT)
        set(CMAKE_CXX_FLAGS_RELWITHDEBUGINFO /MT)
    endif ()
endif ()

include_directories(src)

add_library(jvm)
target_sources(jvm PRIVATE
        src/jvm/AccessFlags.h
        src/jvm/Class.h
        src/jvm/Class.cc
        src/jvm/Class_Conv.cc
        src/jvm/Class_Parse.cc
        src/jvm/ClassLoader.h
        src/jvm/ClassLoader.cc
        src/jvm/Opcode.h
        src/jvm/Runtime.h
        src/jvm/Runtime.cc
        src/jvm/Type.h
        src/jvm/Type.cc
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
        src/test/TestRuntime.cpp
)
target_link_libraries(test PUBLIC jvm)
find_package(GTest CONFIG REQUIRED)
target_link_libraries(test PRIVATE GTest::gtest)

add_custom_command(
        TARGET test
        POST_BUILD
        COMMAND javac "${CMAKE_SOURCE_DIR}/src/test/resource/Hello.java"
        COMMAND javac "${CMAKE_SOURCE_DIR}/src/test/resource/World.java"
        COMMAND javac "${CMAKE_SOURCE_DIR}/src/test/resource/PrimeCalculator.java"
        COMMAND javac "${CMAKE_SOURCE_DIR}/src/test/resource/PiCalculator.java"
)
target_compile_definitions(test PRIVATE "PATH_TO_HELLO_CLASS=\"${CMAKE_SOURCE_DIR}/src/test/resource/Hello.class\"")
target_compile_definitions(test PRIVATE "PATH_TO_WORLD_CLASS=\"${CMAKE_SOURCE_DIR}/src/test/resource/World.class\"")
target_compile_definitions(test PRIVATE "PATH_TO_PRIME_CALCULATOR_CLASS=\"${CMAKE_SOURCE_DIR}/src/test/resource/PrimeCalculator.class\"")
target_compile_definitions(test PRIVATE "PATH_TO_PI_CALCULATOR_CLASS=\"${CMAKE_SOURCE_DIR}/src/test/resource/PiCalculator.class\"")
