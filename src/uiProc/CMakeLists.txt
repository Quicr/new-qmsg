cmake_minimum_required(VERSION 3.10)

project(uiProc)

add_executable(uiProc
    uiProc.cxx
    Channel.cxx
    FdReader.cxx
    Parser.cxx
    Profile.cxx
    Receiver.cxx
    Sender.cxx
    Team.cxx
    UserInterface.cxx)

target_link_libraries(uiProc
    PRIVATE
        qmsgEncoder)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
