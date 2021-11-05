cmake_minimum_required(VERSION 3.20)
project(s-expression)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SRC sexpresso/*.h sexpresso/*.cpp
        sexpresso/detail/*.h sexpresso/detail/*.cpp
        )


include_directories(sexpresso)

add_executable(lexer-test tests/testLexer.cpp ${SRC} sexpresso/detail/SData.h sexpresso/detail/SQueryParser.cpp sexpresso/detail/SQueryParser.h)

add_executable(sdoc-test tests/testSDocument.cpp ${SRC})

add_executable(query-test tests/testQuery.cpp ${SRC} )

add_executable(main main.cpp ${SRC})