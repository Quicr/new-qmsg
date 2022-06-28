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

    TEST_F(QMsgEncoderTest, InitandDeinit)
    {
        QMsgEncoderContext *context = nullptr;

        ASSERT_EQ(QMsgEncoderInit(&context), 0);
        ASSERT_TRUE(context != nullptr);
        QMsgEncoderDeinit(context);
    };

/*
typedef struct QMsgUISendASCIIMsg_t
{
    uint32_t team_id;
    uint32_t channel_id;
    char *message;
} QMsgUISendASCIIMsg_t;
*/

    TEST_F(QMsgEncoderTest, UISendASCIIMessage)
    {
        char expected[] =
        {
            // Message type
            0x00, 0x00, 0x00, 0x08,

            // Message length
            0x00, 0x00, 0x00, 0x16,

            // Team ID + Channel ID
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,

            // Hello, World\0
            0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57,
            0x6f, 0x72, 0x6c, 0x64, 0x21, 0x00
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

} // namespace
