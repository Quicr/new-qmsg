# Consider GTest only when building tests
if(qmsg_BUILD_TESTS)
    add_subdirectory(googletest)
endif()

# quicr
if(qmsg_ENABLE_QUICR)
    add_subdirectory(quicr)
endif()
