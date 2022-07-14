cmake_minimum_required(VERSION 3.17)

# Building tests by default depends on whether this is a subproject
if (DEFINED PROJECT_NAME)
    option(qmsg_BUILD_TESTS "Build Tests for QMsg" OFF)
else()
    option(qmsg_BUILD_TESTS "Build Tests QMsg" ON)
endif()

# Option to control component installation
option(qmsg_INSTALL "Install the QMsg Components" ON)

# Option to specify building "slow-relay"
option(qmsg_SLOW_RELAY "Build the slow-relay project" OFF)

project(QMsg
        VERSION 1.0.0.0
        DESCRIPTION "QMsg"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(contrib)

if (qmsg_SLOW_RELAY)
    add_subdirectory(lib/slower)
    add_subdirectory(lib/qmsgEncoder)
else()
    add_subdirectory(lib/slower)
    add_subdirectory(lib/qmsgEncoder)

    add_subdirectory(src/uiProc)
    #add_subdirectory(src/secProc)
    add_subdirectory(src/netProc)
endif()

add_subdirectory(src/slowTest)
add_subdirectory(src/slowRelay)

add_subdirectory(src/slowUI )
add_subdirectory(src/slowSec )
add_subdirectory(src/slowNet )

include(CTest)

if(BUILD_TESTING AND qmsg_BUILD_TESTS)
    add_subdirectory(test)
endif()
