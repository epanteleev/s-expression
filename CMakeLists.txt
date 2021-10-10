cmake_minimum_required(VERSION 3.20)
project(s-expression)

set(CMAKE_CXX_STANDARD 20)

file(GLOB FILES sexpresso/*.hpp sexpresso/*.cpp
        )

include_directories(sexpresso)
include_directories(sexpresso/fmt-8.0.1/include)
add_subdirectory(sexpresso/fmt-8.0.1 fmt)

add_executable(${PROJECT_NAME}-test tests/test_sexpresso.cpp ${FILES})

target_link_libraries(${PROJECT_NAME}-test fmt::fmt)