/*
 *  net_types.cpp
 *
 *  Copyright (C) 2022
 *  Cisco Systems, Inc.
 *  All Rights Reserved
 *
 *  Description:
 *      This file defines the data structures used by the QMsg Encoder
 *      between the Network Processor and the Secure Processor.
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

// Define the Net<=>Sec message type values
typedef enum QMsgNetMessageType
{
    QMsgNetInvalid = 0,
    QMsgNetSendASCIIMessage,
    QMsgNetReceiveASCIIMessage,
    QMsgNetWatchDevices,
    QMsgNetUnwatchDevices,
    QMsgNetMLSSignatureHash,
    QMsgNetMLSKeyPackage,
    QMsgNetMLSAddKeyPackage,
    QMsgNetMLSWelcome,
    QMsgNetMLSCommit,
    QMsgNetDeviceInfo,
    QMsgNet_RESERVED_RANGE
} QMsgNetMessageType;

// Define the various Net<=>Sec Message types
typedef struct QMsgNetSendASCIIMessage_t
{
    QMsgTeamID team_id;
    QMsgChannelID channel_id;
    QMsgDeviceID  device_id;
    QMsgOpaque_t message;
} QMsgNetSendASCIIMessage_t;

typedef struct QMsgNetReceiveASCIIMessage_t
{
    QMsgTeamID team_id;
    QMsgChannelID channel_id;
    QMsgDeviceID device_id;
    QMsgOpaque_t message;
} QMsgNetReceiveASCIIMessage_t;

typedef struct QMsgNetWatchDevices_t
{
    QMsgTeamID team_id;
    QMsgChannelID channel_id;
    QMsgDeviceList_t device_list;
} QMsgNetWatchDevices_t;

typedef struct QMsgNetUnwatchDevices_t
{
    QMsgTeamID team_id;
    QMsgChannelID channel_id;
    QMsgDeviceList_t device_list;
} QMsgNetUnwatchDevices_t;

typedef struct QMsgNetMLSSignatureHash_t
{
    QMsgTeamID team_id;
    QMsgOpaque_t hash;
} QMsgNetMLSSignatureHash_t;

typedef struct QMsgNetMLSKeyPackage_t
{
    QMsgTeamID team_id;
    QMsgOpaque_t key_package;
    QMsgOpaque_t key_package_hash;
} QMsgNetMLSKeyPackage_t;

typedef struct QMsgNetMLSAddKeyPackage_t
{
    QMsgTeamID team_id;
    QMsgOpaque_t key_package;
} QMsgNetMLSAddKeyPackage_t;

typedef struct QMsgNetMLSWelcome_t
{
    QMsgTeamID team_id;
    QMsgOpaque_t welcome;
} QMsgNetMLSWelcome_t;

typedef struct QMsgNetMLSCommit_t
{
    QMsgTeamID team_id;
    QMsgOpaque_t commit;
} QMsgNetMLSCommit_t;

typedef struct QMsgNetDeviceInfo_t
{
    QMsgTeamID team_id;
    QMsgDeviceID device_id;
} QMsgNetDeviceInfo_t;

// The following union will hold the contents of a single Net<=>Sec message
typedef struct QMsgNetMessage
{
    QMsgNetMessageType type;
    union
    {
        QMsgNetSendASCIIMessage_t send_ascii_message;
        QMsgNetReceiveASCIIMessage_t receive_ascii_message;
        QMsgNetWatchDevices_t watch_devices;
        QMsgNetUnwatchDevices_t unwatch_devices;
        QMsgNetMLSSignatureHash_t mls_signature_hash;
        QMsgNetMLSKeyPackage_t mls_key_package;
        QMsgNetMLSAddKeyPackage_t mls_add_key_package;
        QMsgNetMLSWelcome_t mls_welcome;
        QMsgNetMLSCommit_t mls_commit;
        QMsgNetDeviceInfo_t device_info;
    } u;
} QMsgNetMessage;

#ifdef __cplusplus
} // extern "C"
#endif
