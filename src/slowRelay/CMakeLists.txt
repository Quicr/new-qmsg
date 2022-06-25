cmake_minimum_required(VERSION 3.10 )

project( slowRelay )

set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable( slowRelay slowRelay.cxx )

target_link_libraries( slowRelay LINK_PUBLIC slower )

