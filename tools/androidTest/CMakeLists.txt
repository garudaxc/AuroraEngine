cmake_minimum_required(VERSION 3.4.1)

# now build app's shared lib
# with c++14
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall")

add_library(testcpp11 SHARED  testcpp11.cpp)

# add lib dependencies
target_link_libraries(testcpp11
                      android
                      log 
                      EGL
                      GLESv2)

