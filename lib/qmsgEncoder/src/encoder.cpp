/*
 *  encoder.cpp
 *
 *  Copyright (C) 2022
 *  Cisco Systems, Inc.
 *  All Rights Reserved
 *
 *  Description:
 *      The QMsg Encoder implements functions to convert data structures into
 *      binary objects that can be transmitted between QMsg entities.
 *
 *  Portability Issues:
 *      None.
 */

#include <cstring>
#include "qmsg/encoder.h"
#include "encoder_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 *  QMsgEncoderInit
 *
 *  Description:
 *      Initialize the QMsgEncoder library, populating internal structures.
 *
 *  Parameters:
 *      context [out]
 *          On success, this context pointer will point to the context used
 *          with subsequent library calls.
 *
 *  Returns:
 *      0 if successful, -1 if there was an error.  On error, one should assume
 *      the context pointer is invalid.
 *
 *  Comments:
 *      None.
 */
int CALL QMsgEncoderInit(QMsgEncoderContext **context)
{
    int result = -1;
    qmsg::QMsgEncoderContextInternal *internal_context = nullptr;

    try
    {
        // Create the internal context
        internal_context = new qmsg::QMsgEncoderContextInternal();

        // Create the external context
        *context = new QMsgEncoderContext{nullptr};

        // Assign the opaque pointer to the internal context
        (*context)->opaque = internal_context;

        result = 0;
    }
    catch (...)
    {
        // Free the allocated memory on failure
        if (internal_context) delete internal_context;
    }

    return result;
}

/*
 *  QMsgEncoderDeinit
 *
 *  Description:
 *      Deinitialize the QMsgEncoder library associated with the given context.
 *
 *  Parameters:
 *      context [in]
 *          The context used with the encoder to deinitialize.
 *
 *  Returns:
 *      Nothing, though the caller should assume the context pointer is invalid
 *      once this function is called.
 *
 *  Comments:
 *      None.
 */
void CALL QMsgEncoderDeinit(QMsgEncoderContext *context)
{
    // Just return if the context is a nullptr
    if (!context || !context->opaque) return;

    try
    {
        qmsg::QMsgEncoderContextInternal *internal_context =
            reinterpret_cast<qmsg::QMsgEncoderContextInternal *>(
                context->opaque);

        // Delete the internal context object
        delete internal_context;

        // Delete the external context object
        delete context;
    }
    catch (...)
    {
        // Nothing we can do here
    }
}

/*
 *  QMsgUIEncodeMessage
 *
 *  Description:
 *      Encode the given message into the given buffer.
 *
 *  Parameters:
 *      context [in]
 *          The encoder context to utilize.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *      buffer [out]
 *          The pre-allocated buffer into which the message will be serialized.
 *
 *      buffer_length [in]
 *          The length of the buffer.
 *
 *      encoded_length [out]
 *          The length of the data serialized into the buffer.
 *
 *  Returns:
 *      This function will return one of several values of type
 *      QMsgEncoderResult.  The output arguments are valid only if
 *      QMsgEncoderSuccess is returned.
 *
 *  Comments:
 *      None.
 */
QMsgEncoderResult CALL QMsgUIEncodeMessage(QMsgEncoderContext *context,
                                           const QMsgUIMessage *message,
                                           char *buffer,
                                           size_t buffer_length,
                                           size_t *encoded_length)
{
    // Ensure the context is not null
    if (!context || !context->opaque) return QMsgEncoderInvalidContext;

    // Ensure there is a message
    if (!message) return QMsgEncoderInvalidMessage;

    // Ensure there is a buffer
    if (!buffer || !buffer_length) return QMsgEncoderShortBuffer;

    try
    {
        // Get the encoder context
        qmsg::QMsgEncoderContextInternal *internal_context =
            reinterpret_cast<qmsg::QMsgEncoderContextInternal *>(
                context->opaque);

        // Assign the buffer to a DataBuffer object
        qmsg::DataBuffer data_buffer(reinterpret_cast<unsigned char *>(buffer),
                                     buffer_length,
                                     0);

        // Get a reference to the serializer
        auto &serializer = internal_context->GetSerializer();

        // Serialize the message
        switch (message->type)
        {
            case QMsgUIUnlock:
                *encoded_length = serializer.Serialize(data_buffer,
                                                       message->u.unlock);
                break;

            case QMsgUIIsLocked:
                *encoded_length = serializer.Serialize(data_buffer,
                                                       message->u.is_locked);
                break;

            case QMsgUIDeviceInfo:
                *encoded_length = serializer.Serialize(data_buffer,
                                                       message->u.device_info);
                break;

            case QMsgUIGetTeams:
                *encoded_length = serializer.Serialize(data_buffer,
                                                       message->u.get_teams);
                break;

            case QMsgUITeamInfo:
                *encoded_length = serializer.Serialize(data_buffer,
                                                       message->u.teams_info);
                break;

            case QMsgUIGetChannels:
                *encoded_length = serializer.Serialize(data_buffer,
                                                       message->u.get_channels);
                break;

            case QMsgUIChannelInfo:
                *encoded_length = serializer.Serialize(data_buffer,
                                                       message->u.channel_info);
                break;

            case QMsgUISendASCIIMsg:
                *encoded_length = serializer.Serialize(
                    data_buffer,
                    message->u.send_ascii_message);
                break;

            case QMsgUIReceiveASCIIMessage:
                *encoded_length = serializer.Serialize(
                    data_buffer,
                    message->u.receive_ascii_message);
                break;

            case QMsgUIWatch:
                *encoded_length = serializer.Serialize(data_buffer,
                                                       message->u.watch);
                break;

            case QMsgUIUnwatch:
                *encoded_length = serializer.Serialize(data_buffer,
                                                       message->u.unwatch);
                break;

            case QMsgUIRequestMessages:
                *encoded_length = serializer.Serialize(
                    data_buffer,
                    message->u.request_messages);
                break;

            default:
                return QMsgEncoderInvalidMessage;
        }
    }
    catch (const qmsg::DataBufferException &)
    {
        return QMsgEncoderShortBuffer;
    }
    catch (...)
    {
        return QMsgEncoderUnknownError;
    }

    if (*encoded_length == 0) return QMsgEncoderUnknownError;

    return QMsgEncoderSuccess;
}

/*
 *  QMsgUIDecodeMessage
 *
 *  Description:
 *      Decode a single message from the given buffer, if a whole message
 *      can be found.  If a message is deserialized, there may be additional
 *      octets in the buffer that are not consumed, as they are part of
 *      a next message.  If the deserialization is successful, the number
 *      of octets consumed will be returned in octets_consumed.
 *
 *  Parameters:
 *      context [in]
 *          The encoder context to utilize.
 *
 *      buffer [in]
 *          The buffer from which a message will be deserialized.
 *
 *      buffer_length [in]
 *          The length of the buffer.  This should be the length of the
 *          message contents in the buffer, not the storage allocated for
 *          the buffer.
 *
 *      message [out]
 *          The message deserialized from the buffer.
 *
 *      octets_consumed [out]
 *          The number of octets consumed in the buffer.
 *
 *  Returns:
 *      This function will return one of several values of type
 *      QMsgEncoderResult.  If the buffer contains data, but not a complete
 *      message, that will appear as a QMsgEncoderShortBuffer error.  The
 *      output arguments are valid only if QMsgEncoderSuccess is returned.
 *
 *      If the return type is QMsgEncoderInvalidMessage, this may indicate
 *      either a corrupt buffer or simply an unknown message type.  In this
 *      case, octets_consumed will contain how many octets to advance the
 *      buffer in order to skip this invalid message.  Further,
 *      message.type will be set to QMsgUIInvalid.
 *
 *  Comments:
 *      None.
 */
EXPORT QMsgEncoderResult CALL QMsgUIDecodeMessage(QMsgEncoderContext *context,
                                                  char *buffer,
                                                  size_t buffer_length,
                                                  QMsgUIMessage *message,
                                                  size_t *octets_consumed)
{
    std::uint32_t message_length;               // Expected message length
    std::size_t deserialized;                   // Octets actually deserialized

    // Ensure the context is not null
    if (!context || !context->opaque) return QMsgEncoderInvalidContext;

    // Ensure there is a message structure
    if (!message) return QMsgEncoderInvalidMessage;

    // Ensure there is a buffer
    if (!buffer || !buffer_length) return QMsgEncoderShortBuffer;

    try
    {
        // Get the encoder context
        qmsg::QMsgEncoderContextInternal *internal_context =
            reinterpret_cast<qmsg::QMsgEncoderContextInternal *>(
                context->opaque);

        // Assign the buffer to a DataBuffer object
        qmsg::DataBuffer data_buffer(reinterpret_cast<unsigned char *>(buffer),
                                     buffer_length,
                                     buffer_length);

        // Get a reference to the deserializer
        auto &deserializer = internal_context->GetDeserializer();

        // Zero the message structure
        std::memset(message, 0, sizeof(QMsgUIMessage));

        // Extract the message type and length
        *octets_consumed = deserializer.DeserializeUIMessageType(data_buffer,
                                                                 message->type);

        // Determine the length of the message
        *octets_consumed +=
                    deserializer.DeserializeMessageLength(data_buffer,
                                                          message_length);

        // Is the buffer sufficiently long?
        if (buffer_length < message_length + *octets_consumed)
        {
            return QMsgEncoderShortBuffer;
        }

        // We will assume that all of the message will be consumed
        *octets_consumed += message_length;

        // For unknown message types, return how many octets would have been
        // consumed had the message been processed
        if (message->type == QMsgUIInvalid) return QMsgEncoderInvalidMessage;

        // Deserialize the message
        switch (message->type)
        {
            case QMsgUIUnlock:
                deserialized = deserializer.Deserialize(data_buffer,
                                                        message->u.unlock);
                break;

            case QMsgUIIsLocked:
                deserialized = deserializer.Deserialize(data_buffer,
                                                        message->u.is_locked);
                break;

            case QMsgUIDeviceInfo:
                deserialized = deserializer.Deserialize(data_buffer,
                                                        message->u.device_info);
                break;

            case QMsgUIGetTeams:
                deserialized = deserializer.Deserialize(data_buffer,
                                                        message->u.get_teams);
                break;

            case QMsgUITeamInfo:
                deserialized = deserializer.Deserialize(data_buffer,
                                                        message->u.teams_info);
                break;

            case QMsgUIGetChannels:
                deserialized = deserializer.Deserialize(
                    data_buffer,
                    message->u.get_channels);
                break;

            case QMsgUIChannelInfo:
                deserialized = deserializer.Deserialize(
                    data_buffer,
                    message->u.channel_info);
                break;

            case QMsgUISendASCIIMsg:
                deserialized = deserializer.Deserialize(
                    data_buffer,
                    message->u.send_ascii_message);
                break;

            case QMsgUIReceiveASCIIMessage:
                deserialized = deserializer.Deserialize(
                    data_buffer,
                    message->u.receive_ascii_message);
                break;

            case QMsgUIWatch:
                deserialized = deserializer.Deserialize(data_buffer,
                                                        message->u.watch);
                break;

            case QMsgUIUnwatch:
                deserialized = deserializer.Deserialize(data_buffer,
                                                        message->u.unwatch);
                break;

            case QMsgUIRequestMessages:
                deserialized = deserializer.Deserialize(
                    data_buffer,
                    message->u.request_messages);
                break;

            default:
                return QMsgEncoderInvalidMessage;
        }

        // If the number of octets deserialized is greater than the advertised
        // message length, it suggests the message is bad or the buffer is
        // corrupt; shorter deserialization is allowed for extensibility (i.e.,
        // newer fields might have been added to the known message structure)
        if (deserialized > message_length) return QMsgEncoderCorruptMessage;
    }
    catch (const qmsg::DataBufferException &)
    {
        // If an exception occurs when deserializing, the issue must be a
        // corrupt or bad message format, since the data buffer length was
        // checked above for sufficient length
        return QMsgEncoderCorruptMessage;
    }
    catch (...)
    {
        return QMsgEncoderUnknownError;
    }

    return QMsgEncoderSuccess;
}

#ifdef __cplusplus
} // extern C
#endif
