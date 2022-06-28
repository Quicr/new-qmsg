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
 *      QMsgEncoderResult.
 *
 *  Comments:
 *      None.
 */
QMsgEncoderResult CALL QMsgUIEncodeMessage(QMsgEncoderContext *context,
                                           QMsgUIMessage *message,
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
        auto serializer = internal_context->GetSerializer();

        // Serialize the message
        switch (message->type)
        {
            case QMsgUIUnlock:
                *encoded_length = serializer.Serialize(data_buffer,
                                                       message->u.unlock);
                break;

            case QMsgUIIsLocked:
                return QMsgEncoderInvalidMessage;
                break;

            case QMsgUIDeviceInfo:
                return QMsgEncoderInvalidMessage;
                break;

            case QMsgUIGetTeams:
                return QMsgEncoderInvalidMessage;
                break;

            case QMsgUITeamInfo:
                return QMsgEncoderInvalidMessage;
                break;

            case QMsgUIGetChannels:
                return QMsgEncoderInvalidMessage;
                break;

            case QMsgUIChannelInfo:
                return QMsgEncoderInvalidMessage;
                break;

            case QMsgUISendASCIIMsg:
                return QMsgEncoderInvalidMessage;
                break;

            case QMsgUIReceiveASCIIMessage:
                return QMsgEncoderInvalidMessage;
                break;

            case QMsgUIWatch:
                return QMsgEncoderInvalidMessage;
                break;

            case QMsgUIUnwatch:
                return QMsgEncoderInvalidMessage;
                break;

            case QMsgUIRequestMessages:
                return QMsgEncoderInvalidMessage;
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

#ifdef __cplusplus
} // extern C
#endif
