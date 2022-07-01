cmake_minimum_required(VERSION 3.10 )

project( slowNet )

add_executable( slowNet slowNet.cxx secApi.cxx )


target_link_libraries( slowNet
    PRIVATE
        qmsgEncoder )


target_link_libraries( slowNet LINK_PUBLIC slower )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
