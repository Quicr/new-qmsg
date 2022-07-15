# Enable fetching content
include(FetchContent)

# Try find GoogleTest on the system
find_package(GTest QUIET)

if(GTest_FOUND)
    message(STATUS "Using GTest installed on the system")
else()
    message(STATUS "Fetching GTest since it was not available on the system")

    # Fetch GTest for library's use
    FetchContent_Declare(googletest
        GIT_REPOSITORY  https://github.com/google/googletest.git
        GIT_TAG         e2239ee6043f73722e7aa812a459f54a28552929 # 1.11.0
    )

    # Prevent overriding parent project's compiler/linker settings (Windows)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Prevent GTest from installing
    set(INSTALL_GTEST OFF CACHE INTERNAL "Disable gtest installation")

    # Make gtest available
    FetchContent_MakeAvailable(googletest)
endif()
