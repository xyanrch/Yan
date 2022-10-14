cmake_minimum_required(VERSION 3.5)
project(compiler)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-std=c++17 ")
#add_definitions(-Wall)
add_library(compiler "")
target_sources(compiler
    PRIVATE
    src/type.cpp
    src/lexer.cpp
    src/parser.cpp
    src/token.cpp
    src/log.cpp
    src/symbol.cpp
    src/AST.cpp
    src/gen_x86.cpp
    src/util.cpp
    )
add_executable(YAN main.cpp)
target_link_libraries(YAN compiler)
                      #include_directories(server)
option(YAN_BUILD_TESTS "Build YAN unit test" OFF)
if(YAN_BUILD_TESTS)
    enable_testing()
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(install_gtest OFF)
    set(install_gmock OFF)
    set(build_gmock ON)
    # This project is tested using GoogleTest.
    add_subdirectory("third_party/googletest")
    set_property(TARGET gtest
                      APPEND PROPERTY COMPILE_OPTIONS -Wno-missing-field-initializers)
    set_property(TARGET gmock
                      APPEND PROPERTY COMPILE_OPTIONS -Wno-missing-field-initializers)
    add_executable(UT unittest/test_lexer.cpp
                        unittest/testAll.cpp)

    target_link_libraries(UT gtest)
    target_link_libraries(UT compiler)
    add_test(NAME UT COMMAND UT)
endif(YAN_BUILD_TESTS)



