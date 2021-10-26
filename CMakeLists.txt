cmake_minimum_required(VERSION 3.20)
project(s-expression)

set(CMAKE_CXX_STANDARD 20)

file(GLOB FILES sexpresso/*.hpp sexpresso/*.cpp
        )

include_directories(sexpresso)

add_executable(${PROJECT_NAME}-test tests/test_sexpresso.cpp ${FILES})

add_executable(lexer-test tests/testLexer.cpp ${FILES})

add_executable(main main.cpp ${FILES})