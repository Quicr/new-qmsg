cmake_minimum_required(VERSION 3.10 )

project(  slower VERSION 0.1.2 DESCRIPTION "slower library" )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories( include )
add_subdirectory( src )

target_include_directories ( slower PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )

