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
 *      This function will return the length of the encoded data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(DataBuffer &data_buffer,
                                      const QMsgUIUnlock_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length += Serialize(null_buffer, message.pin);

    // Now serialize the data in TLV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(QMsgUIUnlock));
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
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
 *      This function will return the length of the encoded data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(DataBuffer &data_buffer,
                                      const QMsgUIIsLocked_t &)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // NOTE: This message has no actual content, thus the length is left at 0

    // Now serialize the data in TLV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(QMsgUIIsLocked));
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));

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
 *      This function will return the length of the encoded data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(DataBuffer &data_buffer,
                                      const QMsgUIDeviceInfo_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length += Serialize(null_buffer, message.domain);
    message_length += Serialize(null_buffer, message.org);
    message_length += Serialize(null_buffer, message.kp_fingerprint);

    // Now serialize the data in TLV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(QMsgUIDeviceInfo));
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length += Serialize(data_buffer, message.domain);
    total_length += Serialize(data_buffer, message.org);
    total_length += Serialize(data_buffer, message.kp_fingerprint);

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
 *      This function will return the length of the encoded data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(DataBuffer &data_buffer,
                                      const QMsgUIGetTeams_t &)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // NOTE: This message has no actual content, thus the length is left at 0

    // Now serialize the data in TLV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(QMsgUIGetTeams));
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));

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
 *      This function will return the length of the encoded data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(DataBuffer &data_buffer,
                                      const QMsgUITeamInfo_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.team_name);

    // Now serialize the data in TLV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(QMsgUITeamInfo));
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length += Serialize(data_buffer, message.team_id);
    total_length += Serialize(data_buffer, message.team_name);

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
 *      This function will return the length of the encoded data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(DataBuffer &data_buffer,
                                      const QMsgUIGetChannels_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length += Serialize(null_buffer, message.team_id);

    // Now serialize the data in TLV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(QMsgUIGetChannels));
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length += Serialize(data_buffer, message.team_id);

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
 *      This function will return the length of the encoded data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(DataBuffer &data_buffer,
                                      const QMsgUIChannelInfo_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.channel_id);
    message_length += Serialize(null_buffer, message.channel_name);

    // Now serialize the data in TLV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(QMsgUIChannelInfo));
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length += Serialize(data_buffer, message.team_id);
    total_length += Serialize(data_buffer, message.channel_id);
    total_length += Serialize(data_buffer, message.channel_name);

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
 *      This function will return the length of the encoded data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(DataBuffer &data_buffer,
                                      const QMsgUISendASCIIMsg_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.channel_id);
    message_length += Serialize(null_buffer, message.message);

    // Now serialize the data in TLV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(QMsgUISendASCIIMsg));
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
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
 *      This function will return the length of the encoded data.
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
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.channel_id);
    message_length += Serialize(null_buffer, message.time);
    message_length += Serialize(null_buffer, message.message);

    // Now serialize the data in TLV form
    total_length += Serialize(
        data_buffer,
        static_cast<std::uint32_t>(QMsgUIReceiveASCIIMessage));
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length += Serialize(data_buffer, message.team_id);
    total_length += Serialize(data_buffer, message.channel_id);
    total_length += Serialize(data_buffer, message.time);
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
 *      This function will return the length of the encoded data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(
                                    DataBuffer &data_buffer,
                                    const QMsgUIWatch_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.channel_id);

    // Now serialize the data in TLV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(QMsgUIWatch));
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
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
 *      This function will return the length of the encoded data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(
                                    DataBuffer &data_buffer,
                                    const QMsgUIUnwatch_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.channel_id);

    // Now serialize the data in TLV form
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(QMsgUIUnwatch));
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
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
 *      This function will return the length of the encoded data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(
                                    DataBuffer &data_buffer,
                                    const QMsgUIRequestMessages_t &message)
{
    std::size_t message_length = 0;
    std::size_t total_length = 0;

    // Compute the encoded message length
    message_length += Serialize(null_buffer, message.team_id);
    message_length += Serialize(null_buffer, message.channel_id);
    message_length += Serialize(null_buffer, message.before_time);
    message_length += Serialize(null_buffer, message.message_count);

    // Now serialize the data in TLV form
    total_length += Serialize(
        data_buffer,
        static_cast<std::uint32_t>(QMsgUIRequestMessages));
    total_length += Serialize(data_buffer,
                              static_cast<std::uint32_t>(message_length));
    total_length += Serialize(data_buffer, message.team_id);
    total_length += Serialize(data_buffer, message.channel_id);
    total_length += Serialize(data_buffer, message.before_time);
    total_length += Serialize(data_buffer, message.message_count);

    return total_length;
}

/////////////////////////////////////////////////////

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
 *          The data buffer into which the message shall be serialized.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the encoded data.
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
 *          The data buffer into which the message shall be serialized.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the encoded data.
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
 *          The data buffer into which the message shall be serialized.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the encoded data.
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
 *          The data buffer into which the message shall be serialized.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *  Returns:
 *      This function will return the length of the encoded data.
 *
 *  Comments:
 *      If there is an error writing into the data buffer, the DataBuffer
 *      object will throw an exception.
 */
std::size_t QMsgSerializer::Serialize(DataBuffer &data_buffer,
                                      const char *value)
{
    std::size_t length = strlen(value);

    if (data_buffer.GetBufferSize())
    {
        data_buffer.AppendValue(static_cast<std::uint16_t>(length));
        data_buffer.AppendValue(value, length);
    }

    return sizeof(std::uint16_t) + length;
}

} // namespace qmsg
