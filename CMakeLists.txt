cmake_minimum_required(VERSION 3.26)
project(JNIDirect C)

set(CMAKE_C_STANDARD 11)


include_directories(C:/Users/lidia/.jdks/corretto-1.8.0_372/include)
include_directories(C:/Users/lidia/.jdks/corretto-1.8.0_372/include/win32)

add_library(JNIDirect JNIDirect.c)
