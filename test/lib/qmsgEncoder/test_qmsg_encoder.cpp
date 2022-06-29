/*
 *  test_qmsg_encoder.cpp
 *
 *  Copyright (C) 2022
 *  Cisco Systems, Inc.
 *  All Rights Reserved
 *
 *  Description:
 *      This module will test the QMsgEncoder library
 *
 *  Portability Issues:
 *      None.
 */

#include <cstring>
#include "gtest/gtest.h"
#include "qmsg/encoder.h"

namespace {

    // The fixture for testing the QMsgEncoder library
    class QMsgEncoderTest : public ::testing::Test
    {
        public:
            QMsgEncoderTest() : context{nullptr}
            {
                if (QMsgEncoderInit(&context)) throw "Failed to create context";
            }
            ~QMsgEncoderTest()
            {
                QMsgEncoderDeinit(context);
            }

            bool VerifyDataBuffer(const char expected[], std::size_t length)
            {
                // Verify the buffer contents
                for (std::size_t i = 0; i < length; i++)
                {
                    if (data_buffer[i] != expected[i]) return false;
                }

                return true;
            }

        protected:
            QMsgEncoderContext *context;
            char data_buffer[1500];
    };

    TEST_F(QMsgEncoderTest, InitAndDeinit)
    {
        QMsgEncoderContext *context = nullptr;

        ASSERT_EQ(QMsgEncoderInit(&context), 0);
        ASSERT_TRUE(context != nullptr);
        QMsgEncoderDeinit(context);
    };

    TEST_F(QMsgEncoderTest, Serialize_UISendASCIIMessage)
    {
        char expected[] =
        {
            // Message type
            0x00, 0x00, 0x00, 0x08,

            // Message length
            0x00, 0x00, 0x00, 0x17,

            // Team ID
            0x01, 0x02, 0x03, 0x04,

            // Channel ID
            0x05, 0x06, 0x07, 0x08,

            // String length
            0x00, 0x0d,

            // Hello, World!
            0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57,
            0x6f, 0x72, 0x6c, 0x64, 0x21
        };

        QMsgUIMessage message{};
        char text[] = "Hello, World!";

        message.type = QMsgUISendASCIIMsg;
        message.u.send_ascii_message.team_id = 0x01020304;
        message.u.send_ascii_message.channel_id = 0x05060708;
        message.u.send_ascii_message.message = text;

        std::size_t encoded_length;
        ASSERT_EQ(QMsgUIEncodeMessage(context,
                                      &message,
                                      data_buffer,
                                      sizeof(data_buffer),
                                      &encoded_length),
                  QMsgEncoderSuccess);

        ASSERT_EQ(sizeof(expected), encoded_length);

        ASSERT_TRUE(VerifyDataBuffer(expected, sizeof(expected)));
    };

    TEST_F(QMsgEncoderTest, Deserialize_UISendASCIIMessage)
    {
        char buffer[] =
        {
            // Message type
            0x00, 0x00, 0x00, 0x08,

            // Message length
            0x00, 0x00, 0x00, 0x17,

            // Team ID
            0x01, 0x02, 0x03, 0x04,

            // Channel ID
            0x05, 0x06, 0x07, 0x08,

            // String length
            0x00, 0x0d,

            // Hello, World!
            0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57,
            0x6f, 0x72, 0x6c, 0x64, 0x21
        };

        QMsgUIMessage message{};
        char text[] = "Hello, World!";
        std::size_t bytes_consumed{};

        ASSERT_EQ(QMsgUIDecodeMessage(context,
                                      buffer,
                                      sizeof(buffer),
                                      &message,
                                      &bytes_consumed),
                  QMsgEncoderSuccess);

        ASSERT_EQ(bytes_consumed, sizeof(buffer));
        ASSERT_EQ(message.type, QMsgUISendASCIIMsg);
        ASSERT_EQ(std::uint32_t(0x01020304),
                  message.u.send_ascii_message.team_id);
        ASSERT_EQ(std::uint32_t(0x05060708),
                  message.u.send_ascii_message.channel_id);
        ASSERT_EQ(std::strcmp(message.u.send_ascii_message.message, text), 0);
    };

} // namespace
