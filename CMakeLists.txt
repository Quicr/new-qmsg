cmake_minimum_required(VERSION 3.17)

# Building tests by default depends on whether this is a subproject
if (DEFINED PROJECT_NAME)
    # Option to control building tests
    option(qmsg_BUILD_TESTS "Build Tests for the QMsg" OFF)
else()
    # Option to control building tests
    option(qmsg_BUILD_TESTS "Build Tests for QMsg" ON)
endif()

# Option to control component installation
option(qmsg_INSTALL "Install the QMsg Components" ON)

project(QMsg
        VERSION 1.0.0.0
        DESCRIPTION "Qmsg"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(contrib)

add_subdirectory(lib/slower)
add_subdirectory(lib/qmsgEncoder)


add_subdirectory(src/uiProc)
#add_subdirectory(src/secProc)
add_subdirectory(src/netProc)

add_subdirectory(src/slowTest)
#add_subdirectory(src/slowRelay)

include(CTest)

if(BUILD_TESTING AND qmsg_BUILD_TESTS)
    #add_subdirectory(test)
endif()
