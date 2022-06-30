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
                                           uint8_t *buffer,
                                           size_t buffer_length,
                                           size_t *encoded_length)
{
    // Indicate no data was encoded
    *encoded_length = 0;

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
        qmsg::DataBuffer data_buffer(buffer, buffer_length, 0);

        // Get a reference to the serializer
        auto &serializer = internal_context->GetSerializer();

        // Serialize the message
        switch (message->type)
        {
            case QMsgUISendASCIIMessage:
                *encoded_length = serializer.Serialize(
                    data_buffer,
                    message->u.send_ascii_message);
                break;

            case QMsgUIReceiveASCIIMessage:
                *encoded_length = serializer.Serialize(
                    data_buffer,
                    message->u.receive_ascii_message);
                break;

            case QMsgUIWatchChannel:
                *encoded_length = serializer.Serialize(
                    data_buffer,
                    message->u.watch_channel);
                break;

            case QMsgUIUnwatchChannel:
                *encoded_length = serializer.Serialize(
                    data_buffer,
                    message->u.unwatch_channel);
                break;

            case QMsgUIUnlock:
                *encoded_length = serializer.Serialize(
                    data_buffer,
                    message->u.unlock);
                break;

            case QMsgUIIsLocked:
                *encoded_length = serializer.Serialize(
                    data_buffer,
                    message->u.is_locked);
                break;

            case QMsgUIMLSSignatureHash:
                *encoded_length = serializer.Serialize(
                    data_buffer,
                    message->u.mls_signature_hash);
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
 *      consumed [out]
 *          The number of octets consumed in the buffer.  This will be
 *          set to 0 if no octets were consumed.  If the message is corrupt
 *          or is invalid, this will be set to the length of that bad message
 *          so that the caller may gracefully skip over the message.
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
 *      If the result is QMsgEncoderCorruptMessage, it means the message is
 *      bad (e.g., malformed).  The total octets consumed will allowe the
 *      caller to skip over this bad message.
 *
 *  Comments:
 *      None.
 */
EXPORT QMsgEncoderResult CALL QMsgUIDecodeMessage(QMsgEncoderContext *context,
                                                  uint8_t *buffer,
                                                  size_t buffer_length,
                                                  QMsgUIMessage *message,
                                                  size_t *consumed)
{
    std::uint32_t message_length;               // Expected message length
    std::size_t deserialized;                   // Octets actually deserialized

    // Indicate no data was consumed
    *consumed = 0;

    // Initialize the message type
    message->type = QMsgUIInvalid;

    // Ensure the context is not null
    if (!context || !context->opaque) return QMsgEncoderInvalidContext;

    // Ensure there is a message structure
    if (!message) return QMsgEncoderInvalidMessage;

    // Ensure there is a buffer that at least holds a message length
    if (!buffer || !buffer_length || (buffer_length < sizeof(std::uint32_t)))
    {
        return QMsgEncoderShortBuffer;
    }

    try
    {
        // Get the encoder context
        qmsg::QMsgEncoderContextInternal *internal_context =
            reinterpret_cast<qmsg::QMsgEncoderContextInternal *>(
                context->opaque);

        // Assign the buffer to a DataBuffer object
        qmsg::DataBuffer data_buffer(buffer, buffer_length, buffer_length);

        // Get a reference to the deserializer
        auto &deserializer = internal_context->GetDeserializer();

        // Zero the message structure
        std::memset(message, 0, sizeof(QMsgUIMessage));

        // Determine the length of the message
        *consumed = deserializer.DeserializeMessageLength(data_buffer,
                                                          message_length);

        // If the message length is 0, return an invalid message indicator
        if (message_length == 0) return QMsgEncoderInvalidMessage;

        // Assume that all of the message will be consumed (trailing octets
        // a message structure does not understand will simply be ignored)
        *consumed += message_length;

        // If there is more data in the buffer than one message, adjust the
        // DataBuffer data length to reflect a single message (i.e., do not
        // read into the next message)
        if (*consumed < data_buffer.GetDataLength())
        {
            data_buffer.SetDataLength(*consumed);
        }

        // If the buffer is too short, indicate nothing was consumed and
        // return an error indicating that the buffer is shorter than the
        // total message length
        if (buffer_length < *consumed)
        {
            *consumed = 0;
            return QMsgEncoderShortBuffer;
        }

        // Extract the message type
        deserialized = deserializer.Deserialize(data_buffer, message->type);

        // For unknown message types, return how many octets would have been
        // consumed had the message been processed
        if (message->type == QMsgUIInvalid) return QMsgEncoderInvalidMessage;

        // Deserialize the message
        switch (message->type)
        {
            case QMsgUISendASCIIMessage:
                deserialized += deserializer.Deserialize(
                    data_buffer,
                    message->u.send_ascii_message);
                break;

            case QMsgUIReceiveASCIIMessage:
                deserialized += deserializer.Deserialize(
                    data_buffer,
                    message->u.receive_ascii_message);
                break;

            case QMsgUIWatchChannel:
                deserialized += deserializer.Deserialize(
                    data_buffer,
                    message->u.watch_channel);
                break;

            case QMsgUIUnwatchChannel:
                deserialized += deserializer.Deserialize(
                    data_buffer,
                    message->u.unwatch_channel);
                break;

            case QMsgUIUnlock:
                deserialized += deserializer.Deserialize(
                    data_buffer,
                    message->u.unlock);
                break;

            case QMsgUIIsLocked:
                deserialized += deserializer.Deserialize(
                    data_buffer,
                    message->u.is_locked);
                break;

            case QMsgUIMLSSignatureHash:
                deserialized += deserializer.Deserialize(
                    data_buffer,
                    message->u.mls_signature_hash);
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
    catch (const qmsg::QMsgDeserializerException &)
    {
        // Error indicating the deserializer found a problem a message
        return QMsgEncoderCorruptMessage;
    }
    catch (...)
    {
        // Memory allocation or other failures caught here
        return QMsgEncoderUnknownError;
    }

    return QMsgEncoderSuccess;
}

/*
 *  QMsgNetEncodeMessage
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
QMsgEncoderResult CALL QMsgNetEncodeMessage(QMsgEncoderContext *context,
                                            const QMsgNetMessage *message,
                                            uint8_t *buffer,
                                            size_t buffer_length,
                                            size_t *encoded_length)
{
    // Indicate no data was encoded
    *encoded_length = 0;

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
        qmsg::DataBuffer data_buffer(buffer, buffer_length, 0);

        // Get a reference to the serializer
        auto &serializer = internal_context->GetSerializer();

        // Serialize the message
        switch (message->type)
        {
            case QMsgNetSendASCIIMessage:
                *encoded_length = serializer.Serialize(
                    data_buffer,
                    message->u.send_ascii_message);
                break;

            case QMsgNetReceiveASCIIMessage:
                *encoded_length = serializer.Serialize(
                    data_buffer,
                    message->u.receive_ascii_message);
                break;

            case QMsgNetWatchDevices:
                *encoded_length = serializer.Serialize(
                    data_buffer,
                    message->u.watch_devices);
                break;

            case QMsgNetUnwatchDevices:
                *encoded_length = serializer.Serialize(
                    data_buffer,
                    message->u.unwatch_devices);
                break;

            case QMsgNetMLSSignatureHash:
                *encoded_length = serializer.Serialize(
                    data_buffer,
                    message->u.mls_signature_hash);
                break;

            case QMsgNetMLSKeyPackage:
                *encoded_length = serializer.Serialize(
                    data_buffer,
                    message->u.mls_key_package);
                break;

            case QMsgNetMLSAddKeyPackage:
                *encoded_length = serializer.Serialize(
                    data_buffer,
                    message->u.mls_add_key_package);
                break;

            case QMsgNetMLSWelcome:
                *encoded_length = serializer.Serialize(
                    data_buffer,
                    message->u.mls_welcome);
                break;

            case QMsgNetMLSCommit:
                *encoded_length = serializer.Serialize(
                    data_buffer,
                    message->u.mls_commit);
                break;

            case QMsgNetDeviceInfo:
                *encoded_length = serializer.Serialize(
                    data_buffer,
                    message->u.device_info);
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
 *  QMsgNetDecodeMessage
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
 *      consumed [out]
 *          The number of octets consumed in the buffer.  This will be
 *          set to 0 if no octets were consumed.  If the message is corrupt
 *          or is invalid, this will be set to the length of that bad message
 *          so that the caller may gracefully skip over the message.
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
 *      message.type will be set to QMsgNetInvalid.
 *
 *      If the result is QMsgEncoderCorruptMessage, it means the message is
 *      bad (e.g., malformed).  The total octets consumed will allowe the
 *      caller to skip over this bad message.
 *
 *  Comments:
 *      None.
 */
EXPORT QMsgEncoderResult CALL QMsgNetDecodeMessage(QMsgEncoderContext *context,
                                                   uint8_t *buffer,
                                                   size_t buffer_length,
                                                   QMsgNetMessage *message,
                                                   size_t *consumed)
{
    std::uint32_t message_length;               // Expected message length
    std::size_t deserialized;                   // Octets actually deserialized

    // Indicate no data was consumed
    *consumed = 0;

    // Initialize the message type
    message->type = QMsgNetInvalid;

    // Ensure the context is not null
    if (!context || !context->opaque) return QMsgEncoderInvalidContext;

    // Ensure there is a message structure
    if (!message) return QMsgEncoderInvalidMessage;

    // Ensure there is a buffer that at least holds a message length
    if (!buffer || !buffer_length || (buffer_length < sizeof(std::uint32_t)))
    {
        return QMsgEncoderShortBuffer;
    }

    try
    {
        // Get the encoder context
        qmsg::QMsgEncoderContextInternal *internal_context =
            reinterpret_cast<qmsg::QMsgEncoderContextInternal *>(
                context->opaque);

        // Assign the buffer to a DataBuffer object
        qmsg::DataBuffer data_buffer(buffer, buffer_length, buffer_length);

        // Get a reference to the deserializer
        auto &deserializer = internal_context->GetDeserializer();

        // Zero the message structure
        std::memset(message, 0, sizeof(QMsgNetMessage));

        // Determine the length of the message
        *consumed = deserializer.DeserializeMessageLength(data_buffer,
                                                          message_length);

        // If the message length is 0, return an invalid message indicator
        if (message_length == 0) return QMsgEncoderInvalidMessage;

        // Assume that all of the message will be consumed (trailing octets
        // a message structure does not understand will simply be ignored)
        *consumed += message_length;

        // If there is more data in the buffer than one message, adjust the
        // DataBuffer data length to reflect a single message (i.e., do not
        // read into the next message)
        if (*consumed < data_buffer.GetDataLength())
        {
            data_buffer.SetDataLength(*consumed);
        }

        // If the buffer is too short, indicate nothing was consumed and
        // return an error indicating that the buffer is shorter than the
        // total message length
        if (buffer_length < *consumed)
        {
            *consumed = 0;
            return QMsgEncoderShortBuffer;
        }

        // Extract the message type
        deserialized = deserializer.Deserialize(data_buffer, message->type);

        // For unknown message types, return how many octets would have been
        // consumed had the message been processed
        if (message->type == QMsgNetInvalid) return QMsgEncoderInvalidMessage;

        // Deserialize the message
        switch (message->type)
        {
            case QMsgNetSendASCIIMessage:
                deserialized += deserializer.Deserialize(
                    data_buffer,
                    message->u.send_ascii_message);
                break;

            case QMsgNetReceiveASCIIMessage:
                deserialized += deserializer.Deserialize(
                    data_buffer,
                    message->u.receive_ascii_message);
                break;

            case QMsgNetWatchDevices:
                deserialized += deserializer.Deserialize(
                    data_buffer,
                    message->u.watch_devices);
                break;

            case QMsgNetUnwatchDevices:
                deserialized += deserializer.Deserialize(
                    data_buffer,
                    message->u.unwatch_devices);
                break;

            case QMsgNetMLSSignatureHash:
                deserialized += deserializer.Deserialize(
                    data_buffer,
                    message->u.mls_signature_hash);
                break;

            case QMsgNetMLSKeyPackage:
                deserialized += deserializer.Deserialize(
                    data_buffer,
                    message->u.mls_key_package);
                break;

            case QMsgNetMLSAddKeyPackage:
                deserialized += deserializer.Deserialize(
                    data_buffer,
                    message->u.mls_add_key_package);
                break;

            case QMsgNetMLSWelcome:
                deserialized += deserializer.Deserialize(
                    data_buffer,
                    message->u.mls_welcome);
                break;

            case QMsgNetMLSCommit:
                deserialized += deserializer.Deserialize(
                    data_buffer,
                    message->u.mls_commit);
                break;

            case QMsgNetDeviceInfo:
                deserialized += deserializer.Deserialize(
                    data_buffer,
                    message->u.device_info);
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
    catch (const qmsg::QMsgDeserializerException &)
    {
        // Error indicating the deserializer found a problem a message
        return QMsgEncoderCorruptMessage;
    }
    catch (...)
    {
        // Memory allocation or other failures caught here
        return QMsgEncoderUnknownError;
    }

    return QMsgEncoderSuccess;
}

#ifdef __cplusplus
} // extern C
#endif
