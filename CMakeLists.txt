cmake_minimum_required(VERSION 3.20)
project(s-expression)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SRC sexpresso/*.h sexpresso/*.cpp
        sexpresso/detail/*.h sexpresso/detail/*.cpp
        )

add_compile_options("-pg")

include_directories(sexpresso)

add_executable(${PROJECT_NAME}-test tests/test_sexpresso.cpp ${SRC})

add_executable(lexer-test tests/testLexer.cpp ${SRC} sexpresso/detail/SData.h)

add_executable(sdoc-test tests/testSDocument.cpp ${SRC})

add_executable(query-test tests/testQuery.cpp ${SRC} )

add_link_options("-pg")
add_executable(main main.cpp ${SRC})