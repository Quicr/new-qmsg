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
 *      Messages are encoding using a length-tag-value (TLV) scheme
 *      as follows:
 *          4 octets - message length
 *          4 octets - message type
 *          n octets - encoded message
 *
 *      All numeric values are enoded in network byte order.
 *
 *      For a given context, the library is not thread-safe.  If multiple
 *      threads wish to call into this library simultaneously, each thread
 *      should create its own context via QMsgEncoderInit().  The purpose for
 *      the context is for the library to manage any internally allocated
 *      memory.  At the moment, only the deserializer allocates memory.
 *      Note that the deserializer will free any memory it allocated
 *      previously when the next call is made to decode a message or when
 *      the context is destroyed.
 *
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "ui_types.h"
#include "net_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(_WIN32)
#define EXPORT __declspec(dllexport)
#define CALL __stdcall
#else
#define EXPORT
#define CALL
#endif

typedef enum QMsgEncoderResult
{
    QMsgEncoderSuccess = 0,
    QMsgEncoderBadParameter,
    QMsgEncoderUnknownError,
    QMsgEncoderShortBuffer,
    QMsgEncoderInvalidContext,
    QMsgEncoderInvalidMessage,
    QMsgEncoderCorruptMessage
} QMsgEncoderResult;

// Define an encoder context
typedef struct
{
    void *opaque;
} QMsgEncoderContext;

// Function prototypes to initialize and deinitialize the library
EXPORT int CALL QMsgEncoderInit(QMsgEncoderContext **context);
EXPORT void CALL QMsgEncoderDeinit(QMsgEncoderContext *context);

// Function prototypes for UI<=>Sec message encoding and decoding
EXPORT QMsgEncoderResult CALL QMsgUIEncodeMessage(QMsgEncoderContext *context,
                                                  const QMsgUIMessage *message,
                                                  uint8_t *buffer,
                                                  size_t buffer_length,
                                                  size_t *encoded_length);

EXPORT QMsgEncoderResult CALL QMsgUIDecodeMessage(QMsgEncoderContext *context,
                                                  uint8_t *buffer,
                                                  size_t buffer_length,
                                                  QMsgUIMessage *message,
                                                  size_t *consumed);

// Function prototypes for Net<=>Sec message encoding and decoding
EXPORT QMsgEncoderResult CALL QMsgNetEncodeMessage(QMsgEncoderContext *context,
                                                   const QMsgNetMessage *message,
                                                   uint8_t *buffer,
                                                   size_t buffer_length,
                                                   size_t *encoded_length);

EXPORT QMsgEncoderResult CALL QMsgNetDecodeMessage(QMsgEncoderContext *context,
                                                   uint8_t *buffer,
                                                   size_t buffer_length,
                                                   QMsgNetMessage *message,
                                                   size_t *consumed);

#ifdef __cplusplus
} // extern "C"
#endif
