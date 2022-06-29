# Set the test libraries based on how GTest was made available
find_package(GTest)
if (GTest_FOUND)
    message(STATUS "Tests will link with the system's GTest libraries")
    set(TEST_LIBRARIES GTest::Main GTest::GTest)
else()
    message(STATUS "Tests will link with the dowloaded GTest libraries")
    set(TEST_LIBRARIES gtest gtest_main)
endif()

add_subdirectory(lib)
