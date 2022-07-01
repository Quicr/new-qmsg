/*
 *  serializer.cpp
 *
 *  Copyright (C) 2022
 *  Cisco Systems, Inc.
 *  All Rights Reserved
 *
 *  Description:
 *      This file implements the QMsgSerializer, which will serialize a QMsg
 *      data structure into the given buffer.
 *
 *  Portability Issues:
 *      None.
 */

#include <string.h>
#include "serializer.h"

namespace qmsg
{

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given message into the specified data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the message shall be serialized.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(DataBuffer &data_buffer,
                                      const QMsgUISendASCIIMessage_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length +=
                Serialize(null_buffer,
                          static_cast<std::uint32_t>(QMsgUISendASCIIMessage));
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.channel_id);
    message_length += Serialize(null_buffer, message.message);

    // Now serialize the data in LTV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length +=
                Serialize(data_buffer,
                          static_cast<std::uint32_t>(QMsgUISendASCIIMessage));
    total_length += Serialize(data_buffer, message.team_id);
    total_length += Serialize(data_buffer, message.channel_id);
    total_length += Serialize(data_buffer, message.message);

    return total_length;
}

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given message into the specified data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the message shall be serialized.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(
                                    DataBuffer &data_buffer,
                                    const QMsgUIReceiveASCIIMessage_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length +=
            Serialize(null_buffer,
                      static_cast<std::uint32_t>(QMsgUIReceiveASCIIMessage));
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.channel_id);
    message_length += Serialize(null_buffer, message.device_id);
    message_length += Serialize(null_buffer, message.message);

    // Now serialize the data in LTV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length +=
            Serialize(data_buffer,
                        static_cast<std::uint32_t>(QMsgUIReceiveASCIIMessage));
    total_length += Serialize(data_buffer, message.team_id);
    total_length += Serialize(data_buffer, message.channel_id);
    total_length += Serialize(data_buffer, message.device_id);
    total_length += Serialize(data_buffer, message.message);

    return total_length;
}

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given message into the specified data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the message shall be serialized.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(
                                    DataBuffer &data_buffer,
                                    const QMsgUIWatchChannel_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length += Serialize(null_buffer,
                                static_cast<std::uint32_t>(QMsgUIWatchChannel));
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.channel_id);

    // Now serialize the data in LTV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length +=
            Serialize(data_buffer,
                      static_cast<std::uint32_t>(QMsgUIWatchChannel));
    total_length += Serialize(data_buffer, message.team_id);
    total_length += Serialize(data_buffer, message.channel_id);

    return total_length;
}

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given message into the specified data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the message shall be serialized.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(
                                    DataBuffer &data_buffer,
                                    const QMsgUIUnwatchChannel_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length +=
                Serialize(null_buffer,
                          static_cast<std::uint32_t>(QMsgUIUnwatchChannel));
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.channel_id);

    // Now serialize the data in LTV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length +=
                Serialize(data_buffer,
                          static_cast<std::uint32_t>(QMsgUIUnwatchChannel));
    total_length += Serialize(data_buffer, message.team_id);
    total_length += Serialize(data_buffer, message.channel_id);

    return total_length;
}

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given message into the specified data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the message shall be serialized.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(
                                    DataBuffer &data_buffer,
                                    const QMsgUIUnlock_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length += Serialize(null_buffer,
                                static_cast<std::uint32_t>(QMsgUIUnlock));
    message_length += Serialize(null_buffer, message.pin);

    // Now serialize the data in LTV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(QMsgUIUnlock));
    total_length += Serialize(data_buffer, message.pin);

    return total_length;
}

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given message into the specified data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the message shall be serialized.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(
                            DataBuffer &data_buffer,
                            [[maybe_unused]] const QMsgUIIsLocked_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length += Serialize(null_buffer,
                                static_cast<std::uint32_t>(QMsgUIIsLocked));

    // Now serialize the data in LTV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(QMsgUIIsLocked));

    return total_length;
}

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given message into the specified data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the message shall be serialized.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(
                                    DataBuffer &data_buffer,
                                    const QMsgUIMLSSignatureHash_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length +=
                Serialize(null_buffer,
                          static_cast<std::uint32_t>(QMsgUIMLSSignatureHash));
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.hash);

    // Now serialize the data in LTV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length +=
            Serialize(data_buffer,
                      static_cast<std::uint32_t>(QMsgUIMLSSignatureHash));
    message_length += Serialize(data_buffer, message.team_id);
    message_length += Serialize(data_buffer, message.hash);

    return total_length;
}

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given message into the specified data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the message shall be serialized.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(DataBuffer &data_buffer,
                                      const QMsgNetSendASCIIMessage_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length +=
                Serialize(null_buffer,
                          static_cast<std::uint32_t>(QMsgNetSendASCIIMessage));
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.channel_id);
    message_length += Serialize(null_buffer, message.device_id);
    message_length += Serialize(null_buffer, message.message);

    // Now serialize the data in LTV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length +=
                Serialize(data_buffer,
                          static_cast<std::uint32_t>(QMsgNetSendASCIIMessage));
    total_length += Serialize(data_buffer, message.team_id);
    total_length += Serialize(data_buffer, message.channel_id);
    total_length += Serialize(data_buffer, message.device_id);
    total_length += Serialize(data_buffer, message.message);

    return total_length;
}

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given message into the specified data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the message shall be serialized.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(
                                DataBuffer &data_buffer,
                                const QMsgNetReceiveASCIIMessage_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length +=
            Serialize(null_buffer,
                      static_cast<std::uint32_t>(QMsgNetReceiveASCIIMessage));
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.channel_id);
    message_length += Serialize(null_buffer, message.device_id);
    message_length += Serialize(null_buffer, message.message);

    // Now serialize the data in LTV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length +=
            Serialize(data_buffer,
                      static_cast<std::uint32_t>(QMsgNetReceiveASCIIMessage));
    total_length += Serialize(data_buffer, message.team_id);
    total_length += Serialize(data_buffer, message.channel_id);
    total_length += Serialize(data_buffer, message.device_id);
    total_length += Serialize(data_buffer, message.message);

    return total_length;
}

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given message into the specified data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the message shall be serialized.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(
                                DataBuffer &data_buffer,
                                const QMsgNetWatchDevices_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length +=
                Serialize(null_buffer,
                          static_cast<std::uint32_t>(QMsgNetWatchDevices));
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.channel_id);
    message_length += Serialize(null_buffer, message.device_list);

    // Now serialize the data in LTV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length +=
                Serialize(data_buffer,
                          static_cast<std::uint32_t>(QMsgNetWatchDevices));
    total_length += Serialize(data_buffer, message.team_id);
    total_length += Serialize(data_buffer, message.channel_id);
    total_length += Serialize(data_buffer, message.device_list);

    return total_length;
}

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given message into the specified data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the message shall be serialized.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(
                                DataBuffer &data_buffer,
                                const QMsgNetUnwatchDevices_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length +=
                Serialize(null_buffer,
                          static_cast<std::uint32_t>(QMsgNetUnwatchDevices));
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.channel_id);
    message_length += Serialize(null_buffer, message.device_list);

    // Now serialize the data in LTV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length +=
                Serialize(data_buffer,
                          static_cast<std::uint32_t>(QMsgNetUnwatchDevices));
    total_length += Serialize(data_buffer, message.team_id);
    total_length += Serialize(data_buffer, message.channel_id);
    total_length += Serialize(data_buffer, message.device_list);

    return total_length;
}

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given message into the specified data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the message shall be serialized.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(
                                DataBuffer &data_buffer,
                                const QMsgNetMLSSignatureHash_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length +=
                Serialize(null_buffer,
                          static_cast<std::uint32_t>(QMsgNetMLSSignatureHash));
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.hash);

    // Now serialize the data in LTV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length +=
                Serialize(data_buffer,
                          static_cast<std::uint32_t>(QMsgNetMLSSignatureHash));
    total_length += Serialize(data_buffer, message.team_id);
    total_length += Serialize(data_buffer, message.hash);

    return total_length;
}

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given message into the specified data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the message shall be serialized.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(
                                DataBuffer &data_buffer,
                                const QMsgNetMLSKeyPackage_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length +=
                Serialize(null_buffer,
                          static_cast<std::uint32_t>(QMsgNetMLSKeyPackage));
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.key_package);
    message_length += Serialize(null_buffer, message.key_package_hash);

    // Now serialize the data in LTV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length +=
                Serialize(data_buffer,
                          static_cast<std::uint32_t>(QMsgNetMLSKeyPackage));
    total_length += Serialize(data_buffer, message.team_id);
    total_length += Serialize(data_buffer, message.key_package);
    total_length += Serialize(data_buffer, message.key_package_hash);

    return total_length;
}

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given message into the specified data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the message shall be serialized.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(
                                DataBuffer &data_buffer,
                                const QMsgNetMLSAddKeyPackage_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length +=
                Serialize(null_buffer,
                          static_cast<std::uint32_t>(QMsgNetMLSAddKeyPackage));
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.key_package);

    // Now serialize the data in LTV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length +=
                Serialize(data_buffer,
                          static_cast<std::uint32_t>(QMsgNetMLSAddKeyPackage));
    total_length += Serialize(data_buffer, message.team_id);
    total_length += Serialize(data_buffer, message.key_package);

    return total_length;
}

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given message into the specified data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the message shall be serialized.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(
                                DataBuffer &data_buffer,
                                const QMsgNetMLSWelcome_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length +=
                Serialize(null_buffer,
                          static_cast<std::uint32_t>(QMsgNetMLSWelcome));
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.welcome);

    // Now serialize the data in LTV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length +=
                Serialize(data_buffer,
                          static_cast<std::uint32_t>(QMsgNetMLSWelcome));
    total_length += Serialize(data_buffer, message.team_id);
    total_length += Serialize(data_buffer, message.welcome);

    return total_length;
}

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given message into the specified data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the message shall be serialized.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(
                                DataBuffer &data_buffer,
                                const QMsgNetMLSCommit_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length +=
                Serialize(null_buffer,
                          static_cast<std::uint32_t>(QMsgNetMLSCommit));
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.commit);

    // Now serialize the data in LTV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length +=
                Serialize(data_buffer,
                          static_cast<std::uint32_t>(QMsgNetMLSCommit));
    total_length += Serialize(data_buffer, message.team_id);
    total_length += Serialize(data_buffer, message.commit);

    return total_length;
}

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given message into the specified data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the message shall be serialized.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(
                                DataBuffer &data_buffer,
                                const QMsgNetDeviceInfo_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length +=
                Serialize(null_buffer,
                          static_cast<std::uint32_t>(QMsgNetDeviceInfo));
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.device_id);

    // Now serialize the data in LTV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length +=
                Serialize(data_buffer,
                          static_cast<std::uint32_t>(QMsgNetDeviceInfo));
    total_length += Serialize(data_buffer, message.team_id);
    total_length += Serialize(data_buffer, message.device_id);

    return total_length;
}

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given data type into the specified data buffer.  If the
 *      buffer is an empty buffer, it will compute the length that would be
 *      serialized, but does not actually write into the empty buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the value shall be serialized.
 *
 *      value [in]
 *          The value to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(DataBuffer &data_buffer,
                                      std::uint16_t value)
{
    if (data_buffer.GetBufferSize())
    {
        data_buffer.AppendValue(value);
    }

    return sizeof(std::uint16_t);
}

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given data type into the specified data buffer.  If the
 *      buffer is an empty buffer, it will compute the length that would be
 *      serialized, but does not actually write into the empty buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the value shall be serialized.
 *
 *      value [in]
 *          The value to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(DataBuffer &data_buffer,
                                      std::uint32_t value)
{
    if (data_buffer.GetBufferSize())
    {
        data_buffer.AppendValue(value);
    }

    return sizeof(std::uint32_t);
}

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given data type into the specified data buffer.  If the
 *      buffer is an empty buffer, it will compute the length that would be
 *      serialized, but does not actually write into the empty buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the value shall be serialized.
 *
 *      value [in]
 *          The value to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(DataBuffer &data_buffer,
                                      std::uint64_t value)
{
    if (data_buffer.GetBufferSize())
    {
        data_buffer.AppendValue(value);
    }

    return sizeof(std::uint64_t);
}

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given data type into the specified data buffer.  If the
 *      buffer is an empty buffer, it will compute the length that would be
 *      serialized, but does not actually write into the empty buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the value shall be serialized.
 *
 *      value [in]
 *          The value to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(DataBuffer &data_buffer,
                                      const QMsgOpaque_t &value)
{
    if (data_buffer.GetBufferSize())
    {
        data_buffer.AppendValue(value.length);
        data_buffer.AppendValue(value.data, value.length);
    }

    return sizeof(value.length) + value.length;
}

/*
 *  QMsgSerializer::Serialize
 *
 *  Description:
 *      Serialize the given data type into the specified data buffer.  If the
 *      buffer is an empty buffer, it will compute the length that would be
 *      serialized, but does not actually write into the empty buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer into which the value shall be serialized.
 *
 *      value [in]
 *          The value to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the serialized data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(DataBuffer &data_buffer,
                                      const QMsgDeviceList_t &value)
{
    if (data_buffer.GetBufferSize())
    {
        // Write out octet count, not a device count
        std::uint32_t octets = value.num_devices * sizeof(QMsgDeviceID);
        data_buffer.AppendValue(octets);
        for(std::size_t i = 0; i < value.num_devices; i++)
        {
            data_buffer.AppendValue(value.device_list[i]);
        }
    }

    return sizeof(value.num_devices) +
           (value.num_devices * sizeof(QMsgDeviceID));
}

} // namespace qmsg
