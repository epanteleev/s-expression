cmake_minimum_required(VERSION 3.20)
project(s-expression)

set(CMAKE_CXX_STANDARD 20)

file(GLOB FILES sexpresso/*.h sexpresso/*.cpp
        sexpresso/detail/*.h sexpresso/detail/*.cpp
        )

#add_compile_options("-save-temps" "-O3")

include_directories(sexpresso)

add_executable(${PROJECT_NAME}-test tests/test_sexpresso.cpp ${FILES})

add_executable(lexer-test tests/testLexer.cpp ${FILES} sexpresso/SNode.cpp)

add_executable(sdoc-test tests/testSDocument.cpp ${FILES})

add_executable(main main.cpp ${FILES})