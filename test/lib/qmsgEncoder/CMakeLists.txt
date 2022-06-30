add_executable(test_qmsg_encoder test_qmsg_encoder.cpp)

target_link_libraries(test_qmsg_encoder
    PRIVATE
        qmsgEncoder ${TEST_LIBRARIES})

add_test(NAME test_qmsg_encoder
         COMMAND test_qmsg_encoder)
