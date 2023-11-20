cmake_minimum_required(VERSION 3.26)
project(JNIDirect C)

set(CMAKE_C_STANDARD 11)


#unix
#include_directories(/root/.jdks/corretto-1.8.0_392/include)
#include_directories(/root/.jdks/corretto-1.8.0_392/include/linux)

#windows
include_directories(C:/Users/lidia/.jdks/corretto-1.8.0_372/include)
include_directories(C:/Users/lidia/.jdks/corretto-1.8.0_372/include/win32)




add_library(JNIDirect SHARED JNIDirect.c JNIDirect.h test.c)
