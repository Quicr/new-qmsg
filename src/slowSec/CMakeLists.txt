cmake_minimum_required(VERSION 3.10 )

project( slowSec )

add_executable( slowSec slowSec.cxx  uiApi.cxx netApi.cxx  )

target_link_libraries( slowSec
    PRIVATE
        qmsgEncoder )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
