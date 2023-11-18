cmake_minimum_required(VERSION 3.26)
project(JNIDirect C)

set(CMAKE_C_STANDARD 11)


include_directories(C:/Users/lidia/.jdks/corretto-1.8.0_372/include)
include_directories(C:/Users/lidia/.jdks/corretto-1.8.0_372/include/win32)


set(GCC_COVERAGE_COMPILE_FLAGS "-g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )



add_library(JNIDirect SHARED JNIDirect.c
        test.c
        test.c)
