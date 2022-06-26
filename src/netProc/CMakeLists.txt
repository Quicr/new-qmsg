cmake_minimum_required(VERSION 3.10 )

project( netProc )

include(FetchContent)
FetchContent_Declare(   quicr
        GIT_REPOSITORY      https://github.com/Quicr/libquicr.git
        GIT_TAG             main)
FetchContent_MakeAvailable(quicr)

add_executable( netProc netProc.cxx )
target_link_libraries(netProc quicr)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
