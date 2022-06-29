/*
 *  deserializer.cpp
 *
 *  Copyright (C) 2022
 *  Cisco Systems, Inc.
 *  All Rights Reserved
 *
 *  Description:
 *      This file implements the QMsgDeserializer, which will deserialize a
 *      QMsg data structure from the given buffer.
 *
 *  Portability Issues:
 *      None.
 */

#include <cstdint>
#include "deserializer.h"

namespace qmsg
{

/*
 *  QMsgDeserializer::DeserializeUIMessageType
 *
 *  Description:
 *      This function will extract the type of the message found in the
 *      data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer from which to extract the message type.
 *
 *      type [out]
 *          The type of message found in the buffer.
 *
 *  Returns:
 *      The number of octets read out of the buffer.
 *
 *  Comments:
 *      If there is an error reading the data buffer, the DataBuffer
 *      object will throw an exception.  This function utilizes the ReadValue()
 *      functions in DataBuffer so as to advance the read position in the
 *      buffer.
 *
 *      Note that this, since this is the first function called when
 *      deserializing a new message, this function will take the opportunity
 *      to free and previously allocated memory.
 */
std::size_t QMsgDeserializer::DeserializeUIMessageType(DataBuffer &data_buffer,
                                                       QMsgUIMessageType &type)
{
    std::uint32_t word;

    // Free previous memory allocations
    FreeAllocations();

    // Extract the message type
    data_buffer.ReadValue(word);

    // If the type invalid?
    if (word >= QMsgUI_RESERVED_RANGE)
    {
        type = QMsgUIInvalid;
    }
    else
    {
        type = static_cast<QMsgUIMessageType>(word);
    }

    return sizeof(std::uint32_t);
}

/*
 *  QMsgDeserializer::DeserializeMessageLength
 *
 *  Description:
 *      This function will extract the length of the message from the
 *      data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer from which to extract the message length.
 *
 *      message_length [out]
 *          The length of the message in the buffer.
 *
 *  Returns:
 *      The number of octets read out of the buffer.
 *
 *  Comments:
 *      If there is an error reading the data buffer, the DataBuffer
 *      object will throw an exception.  This function utilizes the ReadValue()
 *      functions in DataBuffer so as to advance the read position in the
 *      buffer.
 */
std::size_t QMsgDeserializer::DeserializeMessageLength(
                                                DataBuffer &data_buffer,
                                                std::uint32_t &message_length)
{
    return Deserialize(data_buffer, message_length);
}

/*
 *  QMsgDeserializer::Deserialize
 *
 *  Description:
 *      This function will deserialize the specified message type from the
 *      buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer from which to extract the message length.
 *
 *      message [out]
 *          The message to deserialize from the data buffer.
 *
 *  Returns:
 *      The number of octets read out of the buffer.
 *
 *  Comments:
 *      If there is an error reading the data buffer, the DataBuffer
 *      object will throw an exception.  This function utilizes the ReadValue()
 *      functions in DataBuffer so as to advance the read position in the
 *      buffer.
 */
std::size_t QMsgDeserializer::Deserialize(DataBuffer &data_buffer,
                                          QMsgUIUnlock_t &message)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    data_buffer.ReadValue(message.pin);

    return data_buffer.GetReadLength() - initial_read_position;
}

/*
 *  QMsgDeserializer::Deserialize
 *
 *  Description:
 *      This function will deserialize the specified message type from the
 *      buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer from which to extract the message length.
 *
 *      message [out]
 *          The message to deserialize from the data buffer.
 *
 *  Returns:
 *      The number of octets read out of the buffer.
 *
 *  Comments:
 *      If there is an error reading the data buffer, the DataBuffer
 *      object will throw an exception.  This function utilizes the ReadValue()
 *      functions in DataBuffer so as to advance the read position in the
 *      buffer.
 */
std::size_t QMsgDeserializer::Deserialize(DataBuffer &, QMsgUIIsLocked_t &)
{
    // This message has no payload
    return 0;
}

/*
 *  QMsgDeserializer::Deserialize
 *
 *  Description:
 *      This function will deserialize the specified message type from the
 *      buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer from which to extract the message length.
 *
 *      message [out]
 *          The message to deserialize from the data buffer.
 *
 *  Returns:
 *      The number of octets read out of the buffer.
 *
 *  Comments:
 *      If there is an error reading the data buffer, the DataBuffer
 *      object will throw an exception.  This function utilizes the ReadValue()
 *      functions in DataBuffer so as to advance the read position in the
 *      buffer.
 */
std::size_t QMsgDeserializer::Deserialize(DataBuffer &data_buffer,
                                          QMsgUIDeviceInfo_t &message)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    Deserialize(data_buffer, message.domain);
    Deserialize(data_buffer, message.org);
    Deserialize(data_buffer, message.kp_fingerprint);

    return data_buffer.GetReadLength() - initial_read_position;
}

/*
 *  QMsgDeserializer::Deserialize
 *
 *  Description:
 *      This function will deserialize the specified message type from the
 *      buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer from which to extract the message length.
 *
 *      message [out]
 *          The message to deserialize from the data buffer.
 *
 *  Returns:
 *      The number of octets read out of the buffer.
 *
 *  Comments:
 *      If there is an error reading the data buffer, the DataBuffer
 *      object will throw an exception.  This function utilizes the ReadValue()
 *      functions in DataBuffer so as to advance the read position in the
 *      buffer.
 */
std::size_t QMsgDeserializer::Deserialize(DataBuffer &, QMsgUIGetTeams_t &)
{
    // This message has no payload
    return 0;
}

/*
 *  QMsgDeserializer::Deserialize
 *
 *  Description:
 *      This function will deserialize the specified message type from the
 *      buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer from which to extract the message length.
 *
 *      message [out]
 *          The message to deserialize from the data buffer.
 *
 *  Returns:
 *      The number of octets read out of the buffer.
 *
 *  Comments:
 *      If there is an error reading the data buffer, the DataBuffer
 *      object will throw an exception.  This function utilizes the ReadValue()
 *      functions in DataBuffer so as to advance the read position in the
 *      buffer.
 */
std::size_t QMsgDeserializer::Deserialize(DataBuffer &data_buffer,
                                          QMsgUITeamInfo_t &message)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    Deserialize(data_buffer, message.team_id);
    Deserialize(data_buffer, message.team_name);

    return data_buffer.GetReadLength() - initial_read_position;
}

/*
 *  QMsgDeserializer::Deserialize
 *
 *  Description:
 *      This function will deserialize the specified message type from the
 *      buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer from which to extract the message length.
 *
 *      message [out]
 *          The message to deserialize from the data buffer.
 *
 *  Returns:
 *      The number of octets read out of the buffer.
 *
 *  Comments:
 *      If there is an error reading the data buffer, the DataBuffer
 *      object will throw an exception.  This function utilizes the ReadValue()
 *      functions in DataBuffer so as to advance the read position in the
 *      buffer.
 */
std::size_t QMsgDeserializer::Deserialize(DataBuffer &data_buffer,
                                          QMsgUIGetChannels_t &message)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    Deserialize(data_buffer, message.team_id);

    return data_buffer.GetReadLength() - initial_read_position;
}

/*
 *  QMsgDeserializer::Deserialize
 *
 *  Description:
 *      This function will deserialize the specified message type from the
 *      buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer from which to extract the message length.
 *
 *      message [out]
 *          The message to deserialize from the data buffer.
 *
 *  Returns:
 *      The number of octets read out of the buffer.
 *
 *  Comments:
 *      If there is an error reading the data buffer, the DataBuffer
 *      object will throw an exception.  This function utilizes the ReadValue()
 *      functions in DataBuffer so as to advance the read position in the
 *      buffer.
 */
std::size_t QMsgDeserializer::Deserialize(DataBuffer &data_buffer,
                                          QMsgUIChannelInfo_t &message)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    Deserialize(data_buffer, message.team_id);
    Deserialize(data_buffer, message.channel_id);
    Deserialize(data_buffer, message.channel_name);

    return data_buffer.GetReadLength() - initial_read_position;
}

/*
 *  QMsgDeserializer::Deserialize
 *
 *  Description:
 *      This function will deserialize the specified message type from the
 *      buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer from which to extract the message length.
 *
 *      message [out]
 *          The message to deserialize from the data buffer.
 *
 *  Returns:
 *      The number of octets read out of the buffer.
 *
 *  Comments:
 *      If there is an error reading the data buffer, the DataBuffer
 *      object will throw an exception.  This function utilizes the ReadValue()
 *      functions in DataBuffer so as to advance the read position in the
 *      buffer.
 */
std::size_t QMsgDeserializer::Deserialize(DataBuffer &data_buffer,
                                          QMsgUISendASCIIMsg_t &message)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    Deserialize(data_buffer, message.team_id);
    Deserialize(data_buffer, message.channel_id);
    Deserialize(data_buffer, message.message);

    return data_buffer.GetReadLength() - initial_read_position;
}

/*
 *  QMsgDeserializer::Deserialize
 *
 *  Description:
 *      This function will deserialize the specified message type from the
 *      buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer from which to extract the message length.
 *
 *      message [out]
 *          The message to deserialize from the data buffer.
 *
 *  Returns:
 *      The number of octets read out of the buffer.
 *
 *  Comments:
 *      If there is an error reading the data buffer, the DataBuffer
 *      object will throw an exception.  This function utilizes the ReadValue()
 *      functions in DataBuffer so as to advance the read position in the
 *      buffer.
 */
std::size_t QMsgDeserializer::Deserialize(DataBuffer &data_buffer,
                                          QMsgUIReceiveASCIIMessage_t &message)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    Deserialize(data_buffer, message.team_id);
    Deserialize(data_buffer, message.channel_id);
    Deserialize(data_buffer, message.time);
    Deserialize(data_buffer, message.message);

    return data_buffer.GetReadLength() - initial_read_position;
}

/*
 *  QMsgDeserializer::Deserialize
 *
 *  Description:
 *      This function will deserialize the specified message type from the
 *      buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer from which to extract the message length.
 *
 *      message [out]
 *          The message to deserialize from the data buffer.
 *
 *  Returns:
 *      The number of octets read out of the buffer.
 *
 *  Comments:
 *      If there is an error reading the data buffer, the DataBuffer
 *      object will throw an exception.  This function utilizes the ReadValue()
 *      functions in DataBuffer so as to advance the read position in the
 *      buffer.
 */
std::size_t QMsgDeserializer::Deserialize(DataBuffer &data_buffer,
                                          QMsgUIWatch_t &message)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    Deserialize(data_buffer, message.team_id);
    Deserialize(data_buffer, message.channel_id);

    return data_buffer.GetReadLength() - initial_read_position;
}

/*
 *  QMsgDeserializer::Deserialize
 *
 *  Description:
 *      This function will deserialize the specified message type from the
 *      buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer from which to extract the message length.
 *
 *      message [out]
 *          The message to deserialize from the data buffer.
 *
 *  Returns:
 *      The number of octets read out of the buffer.
 *
 *  Comments:
 *      If there is an error reading the data buffer, the DataBuffer
 *      object will throw an exception.  This function utilizes the ReadValue()
 *      functions in DataBuffer so as to advance the read position in the
 *      buffer.
 */
std::size_t QMsgDeserializer::Deserialize(DataBuffer &data_buffer,
                                          QMsgUIUnwatch_t &message)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    Deserialize(data_buffer, message.team_id);
    Deserialize(data_buffer, message.channel_id);

    return data_buffer.GetReadLength() - initial_read_position;
}

/*
 *  QMsgDeserializer::Deserialize
 *
 *  Description:
 *      This function will deserialize the specified message type from the
 *      buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer from which to extract the message length.
 *
 *      message [out]
 *          The message to deserialize from the data buffer.
 *
 *  Returns:
 *      The number of octets read out of the buffer.
 *
 *  Comments:
 *      If there is an error reading the data buffer, the DataBuffer
 *      object will throw an exception.  This function utilizes the ReadValue()
 *      functions in DataBuffer so as to advance the read position in the
 *      buffer.
 */
std::size_t QMsgDeserializer::Deserialize(DataBuffer &data_buffer,
                                          QMsgUIRequestMessages_t &message)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    Deserialize(data_buffer, message.team_id);
    Deserialize(data_buffer, message.channel_id);
    Deserialize(data_buffer, message.before_time);
    Deserialize(data_buffer, message.message_count);

    return data_buffer.GetReadLength() - initial_read_position;
}

/*
 *  QMsgDeserializer::Deserialize
 *
 *  Description:
 *      This function will deserialize the given type from the data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer from which to deserialize the given data type.
 *
 *      value [out]
 *          The value to deserialize from the buffer.
 *
 *  Returns:
 *      The number of octets read from the data buffer.
 *
 *  Comments:
 *      If there is an error reading the data buffer, the DataBuffer
 *      object will throw an exception.  This function utilizes the ReadValue()
 *      functions in DataBuffer so as to advance the read position in the
 *      buffer.
 */
std::size_t QMsgDeserializer::Deserialize(DataBuffer &data_buffer,
                                          std::uint16_t &value)
{
    data_buffer.ReadValue(value);

    return sizeof(std::uint16_t);
}

/*
 *  QMsgDeserializer::Deserialize
 *
 *  Description:
 *      This function will deserialize the given type from the data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer from which to deserialize the given data type.
 *
 *      value [out]
 *          The value to deserialize from the buffer.
 *
 *  Returns:
 *      Nothing, though the deserialized value will be stored in the value
 *      argument.
 *
 *  Comments:
 *      If there is an error reading the data buffer, the DataBuffer
 *      object will throw an exception.  This function utilizes the ReadValue()
 *      functions in DataBuffer so as to advance the read position in the
 *      buffer.
 */
std::size_t QMsgDeserializer::Deserialize(DataBuffer &data_buffer,
                                          std::uint32_t &value)
{
    data_buffer.ReadValue(value);

    return sizeof(std::uint32_t);
}

/*
 *  QMsgDeserializer::Deserialize
 *
 *  Description:
 *      This function will deserialize the given type from the data buffer.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer from which to deserialize the given data type.
 *
 *      value [out]
 *          The value to deserialize from the buffer.
 *
 *  Returns:
 *      Nothing, though the deserialized value will be stored in the value
 *      argument.
 *
 *  Comments:
 *      If there is an error reading the data buffer, the DataBuffer
 *      object will throw an exception.  This function utilizes the ReadValue()
 *      functions in DataBuffer so as to advance the read position in the
 *      buffer.
 */
std::size_t QMsgDeserializer::Deserialize(DataBuffer &data_buffer,
                                          std::uint64_t &value)
{
    data_buffer.ReadValue(value);

    return sizeof(std::uint64_t);
}

/*
 *  QMsgDeserializer::Deserialize
 *
 *  Description:
 *      This function will deserialize a C-style string from the data buffer,
 *      allocating memory as required to hold the string.
 *
 *  Parameters:
 *      data_buffer [in]
 *          The data buffer from which to deserialize the given data type.
 *
 *      value [out]
 *          A reference to a pointer that will be assigned an allocated buffer
 *          and populated with the string read from the buffer.  This allocated
 *          memory wll be freed by this library.
 *
 *  Returns:
 *      Nothing, though the value will be populated by this function.
 *
 *  Comments:
 *      If there is an error reading the data buffer, the DataBuffer object
 *      will throw an exception.  This function will advance the data buffer
 *      so that the next read will follow this deserialized value.
 *
 *      An exception may also be thrown if memory allocation fails.
 */
std::size_t QMsgDeserializer::Deserialize(DataBuffer &data_buffer, char *&value)
{
    std::uint16_t string_length;
    std::size_t initial_read_position = data_buffer.GetReadLength();

    // Read the string length
    data_buffer.ReadValue(string_length);

    // Allocate memory for the NULL-terminated string; record pointer
    std::uint8_t *buffer = new std::uint8_t[string_length + 1];
    allocations.push_back(buffer);

    // Assign that pointer to the return value
    value = reinterpret_cast<char *>(buffer);

    // Copy the data
    data_buffer.ReadValue(value, string_length);

    // Set the null terminator octet
    value[string_length] = '\0';

    return data_buffer.GetReadLength() - initial_read_position;
}

/*
 *  QMsgDeserializer::FreeAllocations
 *
 *  Description:
 *      Free any memory previously allocated by the deserializer.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
void QMsgDeserializer::FreeAllocations()
{
    if (allocations.empty()) return;

    for (std::size_t i = 0; i < allocations.size(); i++)
    {
        delete[] allocations[i];
    }

    allocations.clear();
}

} // namespace qmsg
