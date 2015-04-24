cmake_minimum_required(VERSION 3.1)
project(TSP_Christofides)

find_package (Threads REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_BUILD_TYPE Release)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
file(GLOB_RECURSE HEADER_FILES header/*.h header/*.tpp)

add_executable(TSP_Christofides ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(TSP_Christofides ${CMAKE_THREAD_LIBS_INIT})