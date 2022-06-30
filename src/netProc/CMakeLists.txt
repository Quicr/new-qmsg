cmake_minimum_required(VERSION 3.10 )

project( netProc )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(-Wall -pedantic -Wextra -Wmissing-declarations)
elseif(MSVC)
    add_compile_options(/W4 /WX)

    # MSVC helpfully recommends safer equivalents for things like
    # getenv, but they are not portable.
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

include(FetchContent)
FetchContent_Declare(   quicr
        GIT_REPOSITORY      https://github.com/Quicr/libquicr.git
        GIT_TAG             main)
FetchContent_MakeAvailable(quicr)


add_executable( netProc message_loop.cxx Network.cxx netProc.cxx )

target_link_libraries(netProc PRIVATE qmsgEncoder
                            PUBLIC quicr)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
