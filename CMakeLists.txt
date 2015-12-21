cmake_minimum_required(VERSION 2.8.12)
project(skyline)

set(FLAGS -Weverything -pedantic -Werror -std=c++11 -Wno-c++98-compat-pedantic -Wno-padded)
set(COMMON_FILES common.cpp common.hpp)

add_executable(nestedloops ${COMMON_FILES} nestedloops.cpp)
target_compile_options(nestedloops PUBLIC ${FLAGS})

add_executable(noisless ${COMMON_FILES} noisless.cpp)
target_compile_options(noisless PUBLIC ${FLAGS})

add_executable(noisy ${COMMON_FILES} noisy.cpp)
target_compile_options(noisy PUBLIC ${FLAGS})
