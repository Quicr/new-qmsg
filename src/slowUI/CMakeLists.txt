cmake_minimum_required(VERSION 3.10 )

project( slowUI )

add_executable( slowUI slowUI.cxx secApi.cxx )

target_link_libraries( slowUI
    PRIVATE
        qmsgEncoder )


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
