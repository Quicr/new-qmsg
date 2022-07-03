/*
 *  encoder_internal.h
 *
 *  Copyright (C) 2022
 *  Cisco Systems, Inc.
 *  All Rights Reserved
 *
 *  Description:
 *      Type or function definitions used internally by the QMsgEncoder library
 *      that should not be exposed via the public API.
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#include <cstring>
#include <utility>
#include "serializer.h"
#include "deserializer.h"

namespace qmsg
{

// Internal context object
class QMsgEncoderContextInternal
{
    public:
        QMsgEncoderContextInternal() = default;
        ~QMsgEncoderContextInternal() = default;

        QMsgSerializer &GetSerializer() { return serializer; }
        QMsgDeserializer &GetDeserializer() { return deserializer; }

    protected:
        QMsgSerializer serializer;
        QMsgDeserializer deserializer;
};

/*
 *  EncodeCommon
 *
 *  Description:
 *      This function implements basic parameter validation and zeros
 *      out return value types.  This code is common between both the
 *      encoding calls for UI<=>Sec and Net<=>Sec calls.
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
 *      message [in]
 *          The message deserialized from the buffer.
 *
 *      consumed [out]
 *          The number of octets consumed in the buffer.  This will
 *          get initialized to zero in this function.
 *
 *  Returns:
 *      Result of parameter validation.  The caller should return immediately
 *      if the result is anything other than QMsgEncoderSuccess and return the
 *      error to its caller.
 *
 *  Comments:
 *      None.
 */
template <typename T>
std::pair<QMsgEncoderResult, QMsgEncoderContextInternal *>
EncodeCommon(QMsgEncoderContext *context,
             T *message,
             std::uint8_t *buffer,
             std::size_t buffer_length,
             std::size_t *encoded_length)
{
    // Ensure the context is not null
    if (!context || !context->opaque) return {QMsgEncoderInvalidContext, {}};

    // Ensure there is a message
    if (!message) return {QMsgEncoderInvalidMessage, {}};

    // Ensure there is a buffer
    if (!buffer || !buffer_length) return {QMsgEncoderShortBuffer, {}};

    // Ensure the encoded_length argument is not null
    if (!encoded_length) return {QMsgEncoderBadParameter, {}};

    // Initialize the encoded length
    *encoded_length = 0;

    // Get the encoder context
    qmsg::QMsgEncoderContextInternal *internal_context =
        reinterpret_cast<qmsg::QMsgEncoderContextInternal *>(
            context->opaque);

    return {QMsgEncoderSuccess, internal_context};
}

/*
 *  DecodeCommon
 *
 *  Description:
 *      This function implements basic parameter validation and zeros
 *      out return value types.  This code is common between both the
 *      decoding calls for UI<=>Sec and Net<=>Sec calls.
 *
 *  Parameters:
 *      context [in]
 *          The encoder context to utilize.
 *
 *      message [in]
 *          The message to be serialized into the buffer.
 *
 *      buffer [in]
 *          The pre-allocated buffer into which the message will be serialized.
 *
 *      buffer_length [in]
 *          The length of the buffer.
 *
 *      encoded_length [out]
 *          The length of the data serialized into the buffer.  This value
 *          will be initialized to zero by this function.
 *
 *  Returns:
 *      Result of parameter validation.  The caller should return immediately
 *      if the result is anything other than QMsgEncoderSuccess and return the
 *      error to its caller.
 *
 *  Comments:
 *      None.
 */
template <typename T>
std::pair<QMsgEncoderResult, QMsgEncoderContextInternal *>
DecodeCommon(QMsgEncoderContext *context,
             uint8_t *buffer,
             size_t buffer_length,
             T *message,
             size_t *consumed)
{
    // Ensure the context is not null
    if (!context || !context->opaque)
    {
        return {QMsgEncoderInvalidContext, {}};
    }

    // Ensure there is a message structure
    if (!message) return {QMsgEncoderInvalidMessage, {}};

    // Ensure there is a buffer that at least holds a message length
    if (!buffer || !buffer_length || (buffer_length < sizeof(std::uint32_t)))
    {
        return {QMsgEncoderShortBuffer, {}};
    }

    // Ensure the consumed argument is not null
    if (!consumed) return {QMsgEncoderBadParameter, {}};

    // Indicate no data was consumed
    *consumed = 0;

    // Zero the message structure
    std::memset(message, 0, sizeof(T));

    // Get the encoder context
    qmsg::QMsgEncoderContextInternal *internal_context =
        reinterpret_cast<qmsg::QMsgEncoderContextInternal *>(
            context->opaque);

    return {QMsgEncoderSuccess, internal_context};
}

} // namespace qmsg
