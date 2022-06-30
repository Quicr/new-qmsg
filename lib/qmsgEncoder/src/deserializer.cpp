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
 *
 *      Note that this, since this is the first function called when
 *      deserializing a new message, this function will take the opportunity
 *      to free and previously allocated memory.
 */
std::size_t QMsgDeserializer::DeserializeMessageLength(
                                                DataBuffer &data_buffer,
                                                std::uint32_t &message_length)
{
    // Free previous memory allocations
    FreeAllocations();

    return Deserialize(data_buffer, message_length);
}

/*
 *  QMsgDeserializer::Deserialize
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
 */
std::size_t QMsgDeserializer::Deserialize(DataBuffer &data_buffer,
                                          QMsgUIMessageType &type)
{
    std::uint32_t word;

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
                                          QMsgUISendASCIIMessage_t &message)
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
    Deserialize(data_buffer, message.device_id);
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
                                          QMsgUIWatchChannel_t &message)
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
                                          QMsgUIUnwatchChannel_t &message)
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
                                          QMsgUIUnlock_t &message)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    Deserialize(data_buffer, message.pin);

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
std::size_t QMsgDeserializer::Deserialize(
                            [[maybe_unused]] DataBuffer &data_buffer,
                            [[maybe_unused]] QMsgUIIsLocked_t &message)
{
    // Message has no data elements
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
                                          QMsgUIMLSSignatureHash_t &message)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    Deserialize(data_buffer, message.team_id);
    Deserialize(data_buffer, message.hash);

    return data_buffer.GetReadLength() - initial_read_position;
}

/*
 *  QMsgDeserializer::DeserializeNetMessageType
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
 */
std::size_t QMsgDeserializer::Deserialize(DataBuffer &data_buffer,
                                          QMsgNetMessageType &type)
{
    std::uint32_t word;

    // Extract the message type
    data_buffer.ReadValue(word);

    // If the type invalid?
    if (word >= QMsgNet_RESERVED_RANGE)
    {
        type = QMsgNetInvalid;
    }
    else
    {
        type = static_cast<QMsgNetMessageType>(word);
    }

    return sizeof(std::uint32_t);
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
                                          QMsgNetSendASCIIMessage_t &message)
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
                                          QMsgNetReceiveASCIIMessage_t &message)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    Deserialize(data_buffer, message.team_id);
    Deserialize(data_buffer, message.channel_id);
    Deserialize(data_buffer, message.device_id);
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
                                          QMsgNetWatchDevices_t &message)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    Deserialize(data_buffer, message.team_id);
    Deserialize(data_buffer, message.channel_id);
    Deserialize(data_buffer, message.device_list);

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
                                          QMsgNetUnwatchDevices_t &message)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    Deserialize(data_buffer, message.team_id);
    Deserialize(data_buffer, message.channel_id);
    Deserialize(data_buffer, message.device_list);

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
                                          QMsgNetMLSSignatureHash_t &message)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    Deserialize(data_buffer, message.team_id);
    Deserialize(data_buffer, message.hash);

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
                                          QMsgNetMLSKeyPackage_t &message)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    Deserialize(data_buffer, message.team_id);
    Deserialize(data_buffer, message.key_package);

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
                                          QMsgNetMLSAddKeyPackage_t &message)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    Deserialize(data_buffer, message.team_id);
    Deserialize(data_buffer, message.key_package);

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
                                          QMsgNetMLSWelcome_t &message)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    Deserialize(data_buffer, message.team_id);
    Deserialize(data_buffer, message.welcome);

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
                                          QMsgNetMLSCommit_t &message)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    Deserialize(data_buffer, message.team_id);
    Deserialize(data_buffer, message.commit);

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
                                          QMsgNetDeviceInfo_t &message)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    Deserialize(data_buffer, message.team_id);
    Deserialize(data_buffer, message.device_id);

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
 *
 *      An exception may also be thrown if memory allocation fails.
 */
std::size_t QMsgDeserializer::Deserialize(DataBuffer &data_buffer,
                                          QMsgOpaque_t &value)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    data_buffer.ReadValue(value.length);
    value.data = new std::uint8_t[value.length];
    allocations.push_back(value.data);
    data_buffer.ReadValue(reinterpret_cast<char *>(value.data), value.length);

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
 *      Nothing, though the deserialized value will be stored in the value
 *      argument.
 *
 *  Comments:
 *      If there is an error reading the data buffer, the DataBuffer
 *      object will throw an exception.  This function utilizes the ReadValue()
 *      functions in DataBuffer so as to advance the read position in the
 *      buffer.
 *
 *      An exception may also be thrown if memory allocation fails.
 */
std::size_t QMsgDeserializer::Deserialize(DataBuffer &data_buffer,
                                          QMsgDeviceList_t &value)
{
    std::size_t initial_read_position = data_buffer.GetReadLength();

    // The wire encoding holds an octet count, not a device count
    std::uint32_t octets;
    data_buffer.ReadValue(octets);

    // Ensure there is an integral number of devices
    if ((octets % sizeof(QMsgDeviceID)) != 0)
    {
        throw QMsgDeserializerException("Invalid device octet count");
    }

    // Take note of the number of devices
    value.num_devices = octets / sizeof(QMsgDeviceID);

    if (value.num_devices == 0)
    {
        value.device_list = nullptr;
    }
    else
    {
        // Allocate memory for the device list and store it
        std::uint8_t *memory = new std::uint8_t[octets];
        allocations.push_back(memory);

        // Now populate the device list info
        value.device_list = reinterpret_cast<QMsgDeviceID *>(memory);
        for (std::size_t i = 0; i < value.num_devices; i++)
        {
            data_buffer.ReadValue(value.device_list[i]);
        }
    }

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
