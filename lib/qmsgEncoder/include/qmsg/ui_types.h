/*
 *  ui_types.cpp
 *
 *  Copyright (C) 2022
 *  Cisco Systems, Inc.
 *  All Rights Reserved
 *
 *  Description:
 *      This file defines the data structures used by the QMsg Encoder
 *      between the UI Processor to the Secure Processor.
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#include "qmsg_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Define the UI<=>Sec message type values
typedef enum QMsgUIMessageType
{
    QMsgUIInvalid = 0,
    QMsgUISendASCIIMessage,
    QMsgUIReceiveASCIIMessage,
    QMsgUIWatchChannel,
    QMsgUIUnwatchChannel,
    QMsgUIUnlock,
    QMsgUIIsLocked,
    QMsgUIMLSSignatureHash,
    QMsgUI_RESERVED_RANGE
} QMsgUIMessageType;

// Define the various UI<=>Sec Message types
typedef struct QMsgUISendASCIIMessage_t
{
    QMsgTeamID team_id;
    QMsgChannelID channel_id;
    QMsgOpaque_t message;
} QMsgUISendASCIIMessage_t;

typedef struct QMsgUIReceiveASCIIMessage_t
{
    QMsgTeamID team_id;
    QMsgChannelID channel_id;
    QMsgDeviceID device_id;
    QMsgOpaque_t message;
} QMsgUIReceiveASCIIMessage_t;

typedef struct QMsgUIWatchChannel_t
{
    QMsgTeamID team_id;
    QMsgChannelID channel_id;
} QMsgUIWatchChannel_t;

typedef struct QMsgUIUnwatchChannel_t
{
    QMsgTeamID team_id;
    QMsgChannelID channel_id;
} QMsgUIUnwatchChannel_t;

typedef struct QMsgUIUnlock_t
{
    QMsgPin pin;
} QMsgUIUnlock_t;

typedef struct QMsgUIIsLocked_t
{
    void *placeholder;
} QMsgUIIsLocked_t;

typedef struct QMsgUIMLSSignatureHash_t
{
    QMsgTeamID team_id;
    QMsgOpaque_t hash;
} QMsgUIMLSSignatureHash_t;

// The following union will hold the contents of a single UI<=>Sec message
typedef struct QMsgUIMessage
{
    QMsgUIMessageType type;
    union
    {
        QMsgUISendASCIIMessage_t send_ascii_message;
        QMsgUIReceiveASCIIMessage_t receive_ascii_message;
        QMsgUIWatchChannel_t watch_channel;
        QMsgUIUnwatchChannel_t unwatch_channel;
        QMsgUIUnlock_t unlock;
        QMsgUIIsLocked_t is_locked;
        QMsgUIMLSSignatureHash_t mls_signature_hash;
    } u;
} QMsgUIMessage;

#ifdef __cplusplus
} // extern "C"
#endif
