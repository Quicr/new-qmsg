cmake_minimum_required(VERSION 3.10 )

project( slowRelay )

set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB RELAY_SOURCES *.cxx)
file(GLOB RELAY_HEADER *.h)

add_executable( slowRelay  ${RELAY_SOURCES} ${RELAY_HEADERS} )

target_link_libraries( slowRelay LINK_PUBLIC slower )

