cmake_minimum_required(VERSION 2.8.12)
project(skyline)

set(FLAGS -std=c++14 -Weverything -pedantic -Wno-c++98-compat-pedantic -Wno-padded)
set(COMMON_FILES io.cpp io.hpp types.hpp)

add_executable(noisy_skyline ${COMMON_FILES} noisy_skyline.cpp noisy_skyline.hpp)
target_compile_options(noisy_skyline PUBLIC ${FLAGS})

add_executable(nl_skyline ${COMMON_FILES} nl_skyline.cpp nl_skyline.hpp)
target_compile_options(nl_skyline PUBLIC ${FLAGS})
