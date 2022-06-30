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
            // Message length
            0x00, 0x00, 0x00, 0x1d,

            // Message type
            0x00, 0x00, 0x00, 0x01,

            // Team ID
            0x01, 0x02, 0x03, 0x04,

            // Channel ID
            0x05, 0x06, 0x07, 0x08,

            // Opaque data length
            0x00, 0x00, 0x00, 0x0d,

            // Hello, World!
            0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57,
            0x6f, 0x72, 0x6c, 0x64, 0x21
        };

        QMsgUIMessage message{};
        char text[] = "Hello, World!";
        std::uint32_t string_length = strlen(text);

        message.type = QMsgUISendASCIIMessage;
        message.u.send_ascii_message.team_id = 0x01020304;
        message.u.send_ascii_message.channel_id = 0x05060708;
        message.u.send_ascii_message.message.length = string_length;
        message.u.send_ascii_message.message.data =
                                    reinterpret_cast<std::uint8_t *>(text);
        std::memcpy(message.u.send_ascii_message.message.data,
                    text,
                    message.u.send_ascii_message.message.length);

        std::size_t encoded_length;
        ASSERT_EQ(QMsgEncoderSuccess,
                  QMsgUIEncodeMessage(context,
                                      &message,
                                      data_buffer,
                                      sizeof(data_buffer),
                                      &encoded_length));

        ASSERT_EQ(sizeof(expected), encoded_length);

        ASSERT_TRUE(VerifyDataBuffer(expected, sizeof(expected)));
    };

    TEST_F(QMsgEncoderTest, Deserialize_UISendASCIIMessage)
    {
        char buffer[] =
        {
            // Message length
            0x00, 0x00, 0x00, 0x1d,

            // Message type
            0x00, 0x00, 0x00, 0x01,

            // Team ID
            0x01, 0x02, 0x03, 0x04,

            // Channel ID
            0x05, 0x06, 0x07, 0x08,

            // Opaque data length
            0x00, 0x00, 0x00, 0x0d,

            // Hello, World!
            0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57,
            0x6f, 0x72, 0x6c, 0x64, 0x21
        };

        QMsgUIMessage message{};
        char text[] = "Hello, World!";
        std::size_t bytes_consumed{};

        ASSERT_EQ(QMsgEncoderSuccess,
                  QMsgUIDecodeMessage(context,
                                      buffer,
                                      sizeof(buffer),
                                      &message,
                                      &bytes_consumed));

        ASSERT_EQ(sizeof(buffer), bytes_consumed);
        ASSERT_EQ(QMsgUISendASCIIMessage, message.type);
        ASSERT_EQ(std::uint32_t(0x01020304),
                  message.u.send_ascii_message.team_id);
        ASSERT_EQ(std::uint32_t(0x05060708),
                  message.u.send_ascii_message.channel_id);
        ASSERT_EQ(strlen(text), message.u.send_ascii_message.message.length);
        ASSERT_EQ(0,
                  std::memcmp(message.u.send_ascii_message.message.data,
                              text,
                              message.u.send_ascii_message.message.length));
    };

    TEST_F(QMsgEncoderTest, Serialize_NetSendASCIIMessage)
    {
        char expected[] =
        {
            // Message length
            0x00, 0x00, 0x00, 0x1d,

            // Message type
            0x00, 0x00, 0x00, 0x01,

            // Team ID
            0x01, 0x02, 0x03, 0x04,

            // Channel ID
            0x05, 0x06, 0x07, 0x08,

            // Opaque data length
            0x00, 0x00, 0x00, 0x0d,

            // Hello, World!
            0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57,
            0x6f, 0x72, 0x6c, 0x64, 0x21
        };

        QMsgNetMessage message{};
        char text[] = "Hello, World!";
        std::uint32_t string_length = strlen(text);

        message.type = QMsgNetSendASCIIMessage;
        message.u.send_ascii_message.team_id = 0x01020304;
        message.u.send_ascii_message.channel_id = 0x05060708;
        message.u.send_ascii_message.message.length = string_length;
        message.u.send_ascii_message.message.data =
                                    reinterpret_cast<std::uint8_t *>(text);
        std::memcpy(message.u.send_ascii_message.message.data,
                    text,
                    message.u.send_ascii_message.message.length);

        std::size_t encoded_length;
        ASSERT_EQ(QMsgEncoderSuccess,
                  QMsgNetEncodeMessage(context,
                                       &message,
                                       data_buffer,
                                       sizeof(data_buffer),
                                       &encoded_length));

        ASSERT_EQ(sizeof(expected), encoded_length);

        ASSERT_TRUE(VerifyDataBuffer(expected, sizeof(expected)));
    };

    TEST_F(QMsgEncoderTest, Deserialize_NetSendASCIIMessage)
    {
        char buffer[] =
        {
            // Message length
            0x00, 0x00, 0x00, 0x1d,

            // Message type
            0x00, 0x00, 0x00, 0x01,

            // Team ID
            0x01, 0x02, 0x03, 0x04,

            // Channel ID
            0x05, 0x06, 0x07, 0x08,

            // Opaque data length
            0x00, 0x00, 0x00, 0x0d,

            // Hello, World!
            0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57,
            0x6f, 0x72, 0x6c, 0x64, 0x21
        };

        QMsgNetMessage message{};
        char text[] = "Hello, World!";
        std::size_t bytes_consumed{};

        ASSERT_EQ(QMsgEncoderSuccess,
                  QMsgNetDecodeMessage(context,
                                       buffer,
                                       sizeof(buffer),
                                       &message,
                                       &bytes_consumed));

        ASSERT_EQ(sizeof(buffer), bytes_consumed);
        ASSERT_EQ(QMsgUISendASCIIMessage, message.type);
        ASSERT_EQ(std::uint32_t(0x01020304),
                  message.u.send_ascii_message.team_id);
        ASSERT_EQ(std::uint32_t(0x05060708),
                  message.u.send_ascii_message.channel_id);
        ASSERT_EQ(strlen(text), message.u.send_ascii_message.message.length);
        ASSERT_EQ(0,
                  std::memcmp(message.u.send_ascii_message.message.data,
                              text,
                              message.u.send_ascii_message.message.length));
    };


    TEST_F(QMsgEncoderTest, Serialize_NetWatchDevices)
    {
        char expected[] =
        {
            // Message length
            0x00, 0x00, 0x00, 0x16,

            // Message type
            0x00, 0x00, 0x00, 0x03,

            // Team ID
            0x01, 0x02, 0x03, 0x04,

            // Channel ID
            0x05, 0x06, 0x07, 0x08,

            // Device list length (in octets)
            0x00, 0x00, 0x00, 0x06,

            // Device list
            0x00, 0x01, 0x00, 0x02, 0x00, 0x03
        };

        QMsgNetMessage message{};
        QMsgDeviceID devices[] = {1, 2, 3};

        message.type = QMsgNetWatchDevices;
        message.u.watch_devices.team_id = 0x01020304;
        message.u.watch_devices.channel_id = 0x05060708;
        message.u.watch_devices.device_list.num_devices = 3;
        message.u.watch_devices.device_list.device_list = devices;

        std::size_t encoded_length;
        ASSERT_EQ(QMsgEncoderSuccess,
                  QMsgNetEncodeMessage(context,
                                       &message,
                                       data_buffer,
                                       sizeof(data_buffer),
                                       &encoded_length));

        ASSERT_EQ(sizeof(expected), encoded_length);

        ASSERT_TRUE(VerifyDataBuffer(expected, sizeof(expected)));
    };

    TEST_F(QMsgEncoderTest, Deserialize_NetWatchDevices)
    {
        char buffer[] =
        {
            // Message length
            0x00, 0x00, 0x00, 0x16,

            // Message type
            0x00, 0x00, 0x00, 0x03,

            // Team ID
            0x01, 0x02, 0x03, 0x04,

            // Channel ID
            0x05, 0x06, 0x07, 0x08,

            // Device list length (in octets)
            0x00, 0x00, 0x00, 0x06,

            // Device list
            0x00, 0x01, 0x00, 0x02, 0x00, 0x03
        };

        QMsgNetMessage message{};
        std::size_t bytes_consumed{};

        ASSERT_EQ(QMsgEncoderSuccess,
                  QMsgNetDecodeMessage(context,
                                       buffer,
                                       sizeof(buffer),
                                       &message,
                                       &bytes_consumed));

        ASSERT_EQ(sizeof(buffer), bytes_consumed);
        ASSERT_EQ(QMsgNetWatchDevices, message.type);
        ASSERT_EQ(std::uint32_t(0x01020304),
                  message.u.watch_devices.team_id);
        ASSERT_EQ(std::uint32_t(0x05060708),
                  message.u.watch_devices.channel_id);
        ASSERT_EQ(3,
                  message.u.watch_devices.device_list.num_devices);
        ASSERT_EQ(1, message.u.watch_devices.device_list.device_list[0]);
        ASSERT_EQ(2, message.u.watch_devices.device_list.device_list[1]);
        ASSERT_EQ(3, message.u.watch_devices.device_list.device_list[2]);
    };

    TEST_F(QMsgEncoderTest, Deserialize_MultipleMessageExample)
    {
        char buffer[] =
        {
            // MESSAGE #1

            // Message length
            0x00, 0x00, 0x00, 0x1d,

            // Message type
            0x00, 0x00, 0x00, 0x01,

            // Team ID
            0x01, 0x02, 0x03, 0x04,

            // Channel ID
            0x05, 0x06, 0x07, 0x08,

            // Opaque data length
            0x00, 0x00, 0x00, 0x0d,

            // Hello, World!
            0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57,
            0x6f, 0x72, 0x6c, 0x64, 0x21,

            // MESSAGE #2

            // Message length
            0x00, 0x00, 0x00, 0x16,

            // Message type
            0x00, 0x00, 0x00, 0x03,

            // Team ID
            0x01, 0x02, 0x03, 0x04,

            // Channel ID
            0x05, 0x06, 0x07, 0x08,

            // Device list length (in octets)
            0x00, 0x00, 0x00, 0x06,

            // Device list
            0x00, 0x01, 0x00, 0x02, 0x00, 0x03,

            // MESSAGE #3

            // Message length
            0x00, 0x00, 0x00, 0x1d,

            // Message type
            0x00, 0x00, 0x00, 0x01,

            // Team ID
            0x01, 0x02, 0x03, 0x04,

            // Channel ID
            0x05, 0x06, 0x07, 0x08,

            // Opaque data length
            0x00, 0x00, 0x00, 0x0d,

            // Hello, World!
            0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57,
            0x6f, 0x72, 0x6c, 0x64, 0x21,

            // MESSAGE #4 -- partial message!

            // Message length
            0x00, 0x00, 0x00, 0x1d
        };

        QMsgNetMessage message{};
        std::size_t bytes_consumed{};
        std::size_t total_bytes_consumed{};
        QMsgNetMessageType message_types[3] =
        {
            QMsgNetSendASCIIMessage,
            QMsgNetWatchDevices,
            QMsgNetSendASCIIMessage
        };

        for (std::size_t i = 0; i < 3; i++)
        {
            ASSERT_EQ(
                QMsgEncoderSuccess,
                QMsgNetDecodeMessage(context,
                                     buffer + total_bytes_consumed,
                                     sizeof(buffer) - total_bytes_consumed,
                                     &message,
                                     &bytes_consumed));
            ASSERT_EQ(message_types[i], message.type);
            total_bytes_consumed += bytes_consumed;
        }

        // The forth message is incomplete, but there should be 4 octets
        // remaining
        ASSERT_EQ(4, sizeof(buffer) - total_bytes_consumed);

        // Least read should fail with a short buffer error
        ASSERT_EQ(QMsgEncoderShortBuffer,
                  QMsgNetDecodeMessage(context,
                                       buffer + total_bytes_consumed,
                                       sizeof(buffer) - total_bytes_consumed,
                                       &message,
                                       &bytes_consumed));
    };


} // namespace
