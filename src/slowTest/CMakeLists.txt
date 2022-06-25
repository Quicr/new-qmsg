cmake_minimum_required(VERSION 3.10 )

project( slowTest )

set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable( slowTest slowTest.cxx )

target_link_libraries( slowTest LINK_PUBLIC slower )
