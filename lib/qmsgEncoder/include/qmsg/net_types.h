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

#ifdef __cplusplus
extern "C"
{
#endif

// Define the Sec<=>Net message type values
typedef enum QMsgNetMessageType
{
    QMsgNetInvalid = 0,
    QMsgNetDeviceInfo,
    QMsgNetTime,
    QMsgNetMLSCommit,
    QMsgNetWatchTeam,
    QMsgNetMLSWelcome,
    QMsgNetSendASCIIMessage,
    QMsgNetReceiveASCIIMessage,
    QMsgNetGetChannels,
    QMsgNetWatch,
    QMsgNetUnwatch,
    QMsgNetRequestMessages,
    QMsgNet_RESERVED_RANGE
} QMsgNetMessageType;

// The following are the Sec<=>Net message stuctures
typedef struct QMsgNetDeviceInfo_t
{
    uint32_t pin;
    char *domain;
    uint32_t org_id;
    uint32_t device_id;
    char *mls_key_package;
    uint64_t kp_fingerprint;
} QMsgNetDeviceInfo_t;

typedef struct QMsgNetTime_t
{
    uint64_t time;
} QMsgNetTime_t;

typedef struct QMsgNetMLSCommit_t
{
    char *message;
} QMsgNetMLSCommit_t;

typedef struct QMsgNetWatchTeam_t
{
    uint32_t team_id;
} QMsgNetWatchTeam_t;

typedef struct QMsgNetMLSWelcome_t
{
    char *message;
} QMsgNetMLSWelcome_t;

typedef struct QMsgNetSendASCIIMessage_t
{
    char *encrypted_message;
    uint64_t expiry_time;
    uint32_t team_id;
    uint32_t channel_id;
} QMsgNetSendASCIIMessage_t;

typedef struct QMsgNetReceiveASCIIMessage_t
{
    char *encrypted_message;
    uint32_t claimed_team;
    uint32_t claimed_channel;
    uint32_t claimed_device;
} QMsgNetReceiveASCIIMessage_t;

typedef struct QMsgNetGetChannels_t
{
    uint32_t team_id;
} QMsgNetGetChannels_t;

typedef struct QMsgNetWatch_t
{
    uint32_t team_id;
    uint32_t channel_id;
    uint32_t sender_device;
} QMsgNetWatch_t;

typedef struct QMsgNetUnwatch_t
{
    uint32_t team_id;
    uint32_t channel_id;
    uint32_t sender_device;
} QMsgNetUnwatch_t;

typedef struct QMsgNetRequestMessages_t
{
    uint32_t team_id;
    uint32_t channel_id;
    uint32_t sender_device;
    uint64_t before_time;
    uint16_t message_count;
} QMsgNetRequestMessages_t;

// The following union will hold the contents of a single UI message
typedef struct QMsgNetMessage
{
    QMsgNetMessageType type;
    union
    {
        QMsgNetDeviceInfo_t device_info;
        QMsgNetTime_t time;
        QMsgNetMLSCommit_t commit_message;
        QMsgNetWatchTeam_t watch_team;
        QMsgNetMLSWelcome_t welcome_message;
        QMsgNetSendASCIIMessage_t send_ascii_message;
        QMsgNetReceiveASCIIMessage_t receive_ascii_message;
        QMsgNetGetChannels_t get_channels;
        QMsgNetWatch_t watch;
        QMsgNetUnwatch_t unwatch;
        QMsgNetRequestMessages_t request_messages;
    } u;
} QMsgNetMessage;

#ifdef __cplusplus
} // extern "C"
#endif
