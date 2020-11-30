cmake_minimum_required(VERSION 3.0)
project(ip_augment_module)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")

include_directories(headers)

add_subdirectory(lib)

add_library(ip_augment SHARED ip_augment.cc)
target_link_libraries(ip_augment ip2region)
