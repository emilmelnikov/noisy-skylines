cmake_minimum_required (VERSION 2.8)
project (skyline)
file (GLOB SOURCES *.cpp *.hpp)
add_executable (${PROJECT_NAME} ${SOURCES})
target_compile_options (${PROJECT_NAME} PRIVATE -Weverything -pedantic
        -std=c++14 -Wno-c++98-compat -Wno-c++98-compat-pedantic
        -Wno-padded
)
