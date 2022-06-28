add_executable(test_qmsg_encoder test_qmsg_encoder.cpp)

include(GoogleTest)

target_link_libraries(test_qmsg_encoder
    PRIVATE
        qmsgEncoder GTest::GTest GTest::Main)

add_test(NAME test_qmsg_encoder
         COMMAND test_qmsg_encoder)
