cmake_minimum_required(VERSION 3.20)
project(s-expression)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SRC src/*.h src/*.cpp
        src/detail/*.h src/detail/*.cpp
        )

include_directories(src)

add_executable(lexer-test tests/testLexer.cpp ${SRC} src/detail/SData.h src/detail/SQueryParser.cpp src/detail/SQueryParser.h)

add_executable(sdoc-test tests/testSDocument.cpp ${SRC})

add_executable(query-test tests/testQuery.cpp ${SRC})

add_executable(sexpr-test tests/testSexpression.cpp ${SRC})

add_executable(main main.cpp ${SRC})