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

            template<typename T>
            bool VerifyBuffers(const T expected[],
                               const T actual[],
                               std::size_t length)
            {
                // Verify the buffer contents
                for (std::size_t i = 0; i < length; i++)
                {
                    if (expected[i] != actual[i]) return false;
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

        // And it should indicate no octets consumed
        ASSERT_EQ(0, bytes_consumed);
    };

    TEST_F(QMsgEncoderTest, Deserialize_UI_ShortBuffer)
    {
        char buffer[] =
        {
            // Message length
            0x00, 0x00, 0x00, 0x16,

            // Message type
            0x00, 0x00, 0x00, 0x03,
        };

        QMsgUIMessage message{};
        std::size_t bytes_consumed{};

        // Least read should fail with a short buffer error
        ASSERT_EQ(QMsgEncoderShortBuffer,
                  QMsgUIDecodeMessage(context,
                                      buffer,
                                      sizeof(buffer),
                                      &message,
                                      &bytes_consumed));

        // And it should indicate no octets consumed
        ASSERT_EQ(0, bytes_consumed);
    };

    TEST_F(QMsgEncoderTest, Deserialize_Net_ShortBuffer)
    {
        char buffer[] =
        {
            // Message length
            0x00, 0x00, 0x00, 0x16,

            // Message type
            0x00, 0x00, 0x00, 0x03,
        };

        QMsgNetMessage message{};
        std::size_t bytes_consumed{};

        // Least read should fail with a short buffer error
        ASSERT_EQ(QMsgEncoderShortBuffer,
                  QMsgNetDecodeMessage(context,
                                       buffer,
                                       sizeof(buffer),
                                       &message,
                                       &bytes_consumed));

        // And it should indicate no octets consumed
        ASSERT_EQ(0, bytes_consumed);
    };

    TEST_F(QMsgEncoderTest, Deserialize_UI_ReallyShortBuffer)
    {
        char buffer[] =
        {
            // Message length (partial)
            0x00, 0x00
        };

        QMsgUIMessage message{};
        std::size_t bytes_consumed{};

        // Least read should fail with a short buffer error
        ASSERT_EQ(QMsgEncoderShortBuffer,
                  QMsgUIDecodeMessage(context,
                                      buffer,
                                      sizeof(buffer),
                                      &message,
                                      &bytes_consumed));

        // And it should indicate no octets consumed
        ASSERT_EQ(0, bytes_consumed);
    };

    TEST_F(QMsgEncoderTest, Deserialize_Net_ReallyShortBuffer)
    {
        char buffer[] =
        {
            // Message length (partial)
            0x00, 0x00
        };

        QMsgNetMessage message{};
        std::size_t bytes_consumed{};

        // Least read should fail with a short buffer error
        ASSERT_EQ(QMsgEncoderShortBuffer,
                  QMsgNetDecodeMessage(context,
                                       buffer,
                                       sizeof(buffer),
                                       &message,
                                       &bytes_consumed));

        // And it should indicate no octets consumed
        ASSERT_EQ(0, bytes_consumed);
    };

    TEST_F(QMsgEncoderTest, Deserialize_UI_ZeroLengthMessage)
    {
        char buffer[] =
        {
            // Message length
            0x00, 0x00, 0x00, 0x00
        };

        QMsgUIMessage message{};
        std::size_t bytes_consumed{};

        // Least read should fail with a short buffer error
        ASSERT_EQ(QMsgEncoderInvalidMessage,
                  QMsgUIDecodeMessage(context,
                                      buffer,
                                      sizeof(buffer),
                                      &message,
                                      &bytes_consumed));

        // And it should indicate how many octets to advance the buffer (4)
        ASSERT_EQ(sizeof(std::uint32_t), bytes_consumed);
    };

    TEST_F(QMsgEncoderTest, Deserialize_Net_ZeroLengthMessage)
    {
        char buffer[] =
        {
            // Message length
            0x00, 0x00, 0x00, 0x00
        };

        QMsgNetMessage message{};
        std::size_t bytes_consumed{};

        // Least read should fail with a short buffer error
        ASSERT_EQ(QMsgEncoderInvalidMessage,
                  QMsgNetDecodeMessage(context,
                                       buffer,
                                       sizeof(buffer),
                                       &message,
                                       &bytes_consumed));

        // And it should indicate how many octets to advance the buffer (4)
        ASSERT_EQ(sizeof(std::uint32_t), bytes_consumed);
    };

    TEST_F(QMsgEncoderTest, Deserialize_Net_Commit)
    {
        std::uint8_t buffer[] =
        {
            0x00, 0x00, 0x02, 0xb8, 0x00, 0x00, 0x00, 0x09,
            0x00, 0x00, 0x00, 0x7b, 0x00, 0x00, 0x02, 0xac,
            0x02, 0x05, 0x74, 0x65, 0x73, 0x74, 0x5f, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
            0x00, 0x00, 0x00, 0x00, 0x28, 0x69, 0xb2, 0x6d,
            0x16, 0x88, 0xbb, 0xfe, 0xdd, 0x74, 0x6c, 0xfe,
            0xab, 0x44, 0xb4, 0x3c, 0x5c, 0xe3, 0xde, 0x5c,
            0xda, 0x86, 0x14, 0x31, 0x9e, 0x4e, 0xd9, 0x55,
            0xc0, 0xd5, 0xba, 0x08, 0x8f, 0x74, 0xac, 0x06,
            0x0a, 0xf2, 0x69, 0x39, 0xf7, 0x00, 0x00, 0x02,
            0x6b, 0x4c, 0xac, 0x92, 0x4e, 0x6c, 0x51, 0x25,
            0x21, 0x02, 0x70, 0xbb, 0x5f, 0x28, 0x00, 0x73,
            0x3c, 0x73, 0x8f, 0xe0, 0x29, 0xc4, 0x7f, 0x26,
            0x97, 0xd4, 0x7a, 0x62, 0xa3, 0xdd, 0xb8, 0xda,
            0xb9, 0x2e, 0xed, 0x19, 0x5e, 0xed, 0xde, 0xdb,
            0x00, 0x24, 0xbb, 0xd6, 0x5b, 0x1b, 0x8d, 0x49,
            0xaa, 0xe7, 0xba, 0xf2, 0xe7, 0xf6, 0x88, 0xaf,
            0x0d, 0x58, 0xcb, 0x9e, 0x7d, 0xfb, 0x33, 0xba,
            0x35, 0x8c, 0x91, 0x84, 0x01, 0xec, 0xa7, 0xb7,
            0xe0, 0x77, 0x5b, 0x7b, 0x32, 0xb2, 0x27, 0x4e,
            0x9e, 0x77, 0x3a, 0xe1, 0xb9, 0x62, 0x4f, 0x1f,
            0x6b, 0x1e, 0x67, 0x35, 0x1e, 0x32, 0x66, 0xc5,
            0x06, 0x5f, 0x2b, 0x58, 0x44, 0xd1, 0xec, 0xaa,
            0x7f, 0x44, 0x48, 0xaa, 0x86, 0xb1, 0x9c, 0x29,
            0xe3, 0x3d, 0xce, 0x53, 0xf2, 0x9b, 0x6c, 0xda,
            0xa5, 0xf0, 0x23, 0xe9, 0xbf, 0xb8, 0x75, 0x49,
            0x42, 0xe7, 0xbf, 0x39, 0xbb, 0x3a, 0x98, 0x52,
            0x4d, 0x67, 0x0f, 0x15, 0x8e, 0xd9, 0x3e, 0x6d,
            0x65, 0xeb, 0xcc, 0xdc, 0x2d, 0xfa, 0x70, 0x46,
            0x11, 0x87, 0x1b, 0x81, 0x24, 0x07, 0x0b, 0xbe,
            0xee, 0x06, 0x33, 0x98, 0x0d, 0x97, 0x94, 0x0f,
            0x3a, 0xd7, 0x91, 0x3b, 0xcf, 0x75, 0xff, 0x6d,
            0xc3, 0x08, 0xaf, 0x65, 0xcc, 0x67, 0xdc, 0xb4,
            0x8e, 0x4e, 0x00, 0x08, 0xd0, 0x1d, 0xbe, 0xc3,
            0x0d, 0x78, 0xd1, 0x0a, 0xf4, 0x65, 0x43, 0x8d,
            0x90, 0x45, 0xe6, 0x5e, 0xa6, 0x07, 0xf4, 0xe0,
            0x80, 0x9a, 0x7d, 0xd3, 0x9e, 0x41, 0x77, 0x0f,
            0xc9, 0x82, 0xaf, 0x62, 0x12, 0x54, 0xf4, 0xc2,
            0xd7, 0xae, 0xfb, 0x69, 0x62, 0x81, 0xaa, 0xe3,
            0xf4, 0x10, 0xea, 0x6e, 0x4f, 0x04, 0xac, 0x89,
            0x1b, 0xe5, 0xf6, 0x59, 0x29, 0x8f, 0xbd, 0xa7,
            0x28, 0xba, 0xa2, 0xd3, 0x8a, 0x14, 0x7b, 0xae,
            0x92, 0x51, 0x69, 0xf8, 0x6d, 0xe4, 0x4c, 0xae,
            0xee, 0x55, 0xe9, 0x46, 0xbc, 0xe8, 0xb1, 0x64,
            0x4f, 0xfb, 0xe0, 0x54, 0xee, 0xb4, 0xc1, 0x01,
            0xf5, 0x26, 0x65, 0xc5, 0x50, 0x0f, 0xc7, 0xa6,
            0xc6, 0x3f, 0xc1, 0x3d, 0x53, 0xec, 0x6d, 0xa0,
            0x86, 0x4f, 0x08, 0x5f, 0x2d, 0x85, 0x61, 0x9c,
            0x67, 0xbc, 0xa5, 0x1d, 0xa1, 0x4a, 0xfc, 0xcb,
            0x78, 0xa0, 0xd2, 0xcc, 0x9e, 0x9c, 0x95, 0x23,
            0xd9, 0xe2, 0x66, 0x89, 0x6e, 0x63, 0xe1, 0x9f,
            0x59, 0xf8, 0xb4, 0xd2, 0xd1, 0x2f, 0xf9, 0x0b,
            0x74, 0x24, 0x14, 0x5c, 0x43, 0x65, 0x94, 0xd8,
            0xa1, 0xf8, 0x02, 0x61, 0x3d, 0xb9, 0x7f, 0xb9,
            0x64, 0xb4, 0x9f, 0x17, 0xbf, 0xdd, 0xe7, 0xdf,
            0x68, 0xc7, 0x49, 0xcd, 0x7e, 0x63, 0x6c, 0xc1,
            0x17, 0x80, 0xcc, 0x21, 0x62, 0x2b, 0x65, 0x09,
            0x46, 0x81, 0xe5, 0xe9, 0x20, 0x06, 0x24, 0x34,
            0x30, 0xdc, 0xb2, 0x9c, 0xc0, 0x80, 0x6e, 0x84,
            0x09, 0x91, 0x18, 0x73, 0x23, 0x18, 0x81, 0xf1,
            0x9b, 0xad, 0x16, 0x2b, 0x23, 0x46, 0x67, 0x5d,
            0xd3, 0xaf, 0x03, 0xdf, 0x29, 0xe3, 0x32, 0x9b,
            0x8d, 0x2f, 0x7a, 0xc6, 0x89, 0x5c, 0x53, 0xe9,
            0x16, 0x3c, 0x73, 0xf4, 0x96, 0xe6, 0xf6, 0x39,
            0x34, 0x00, 0x42, 0xff, 0x23, 0x65, 0x54, 0xc8,
            0xfe, 0xf9, 0xcd, 0x37, 0xf3, 0xb1, 0x7c, 0x02,
            0x4d, 0xc4, 0x5a, 0x92, 0xdd, 0x7b, 0x7d, 0x16,
            0xc8, 0x88, 0xce, 0xaf, 0xb2, 0xba, 0xc3, 0x20,
            0xf7, 0x1b, 0xb1, 0x0e, 0xd9, 0xe9, 0x45, 0x85,
            0xb0, 0x7e, 0x07, 0xc0, 0x5a, 0xf7, 0x53, 0x74,
            0x33, 0x02, 0xff, 0x28, 0xaa, 0xa3, 0x58, 0x30,
            0x7c, 0xcd, 0xb6, 0x72, 0x00, 0xfd, 0xf0, 0x35,
            0x13, 0xaa, 0x77, 0x9c, 0xd3, 0x80, 0x3d, 0xa2,
            0x1b, 0x85, 0x1c, 0x54, 0xb1, 0xa5, 0xf0, 0x16,
            0x72, 0xa2, 0x0a, 0x7b, 0x86, 0xfa, 0xae, 0x8e,
            0xe2, 0x58, 0x13, 0xf2, 0xec, 0x73, 0x09, 0xad,
            0x30, 0x3c, 0xfd, 0x45, 0x3c, 0x09, 0xa1, 0x78,
            0x2a, 0x3c, 0xdf, 0x8d, 0x52, 0xcc, 0x71, 0x04,
            0x3a, 0xca, 0x24, 0x72, 0x1a, 0x4c, 0x12, 0xe5,
            0xf4, 0xb4, 0x40, 0x48, 0x54, 0xae, 0x52, 0x6b,
            0x92, 0xdf, 0x70, 0x2e, 0xb3, 0xf1, 0x9a, 0xa0,
            0x06, 0xd7, 0x93, 0x52, 0xb5, 0xa8, 0xc3, 0x47,
            0xb9, 0x7b, 0x13, 0x77, 0x23, 0x2a, 0x7b, 0xa5,
            0x28, 0x6b, 0x6a, 0x87, 0xaa, 0xb5, 0xb0, 0x4d,
            0x9f, 0x66, 0x09, 0xd9, 0xcf, 0x0b, 0x3c, 0xf0,
            0x57, 0xf2, 0x28, 0x6f, 0x2f, 0x44, 0xbb, 0xf5,
            0xbf, 0x91, 0x3b, 0xfb, 0xaa, 0x33, 0x0b, 0xe6,
            0xc7, 0x50, 0x54, 0x6f
        };

        QMsgNetMessage message{};
        std::size_t bytes_consumed{};

        ASSERT_EQ(QMsgEncoderSuccess,
                  QMsgNetDecodeMessage(context,
                                       reinterpret_cast<char *>(buffer),
                                       sizeof(buffer),
                                       &message,
                                       &bytes_consumed));

        ASSERT_EQ(sizeof(buffer), bytes_consumed);
        ASSERT_EQ(QMsgNetMLSCommit, message.type);
        ASSERT_EQ(123, message.u.mls_commit.team_id);
        ASSERT_EQ(684, message.u.mls_commit.commit.length);
        ASSERT_TRUE(VerifyBuffers(buffer + 16,
                                  message.u.mls_commit.commit.data,
                                  message.u.mls_commit.commit.length));
    }

} // namespace
