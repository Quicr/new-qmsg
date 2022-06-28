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

#ifdef __cplusplus
extern "C"
{
#endif

// Define the UI<=>Sec message type values
typedef enum QMsgUIMessageType
{
    QMsgUIInvalid = 0,
    QMsgUIUnlock,
    QMsgUIIsLocked,
    QMsgUIDeviceInfo,
    QMsgUIGetTeams,
    QMsgUITeamInfo,
    QMsgUIGetChannels,
    QMsgUIChannelInfo,
    QMsgUISendASCIIMsg,
    QMsgUIReceiveASCIIMessage,
    QMsgUIWatch,
    QMsgUIUnwatch,
    QMsgUIRequestMessages,
    QMsgUI_RESERVED_RANGE
} QMsgUIMessageType;

// The following are the UI<=>Sec message stuctures
typedef struct QMsgUIUnlock_t
{
    uint32_t pin;
} QMsgUIUnlock_t;

typedef struct QMsgUIIsLocked_t
{
    int present;
} QMsgUIIsLocked_t;

typedef struct QMsgUIDeviceInfo_t
{
    char *domain;
    uint32_t org;
    uint64_t kp_fingerprint;
} QMsgUIDeviceInfo_t;

typedef struct QMsgUIGetTeams_t
{
    int present;
} QMsgUIGetTeams_t;

typedef struct QMsgUITeamInfo_t
{
    uint32_t team_id;
    char *team_name;
} QMsgUITeamInfo_t;

typedef struct QMsgUIGetChannels_t
{
    uint32_t team_id;
} QMsgUIGetChannels_t;

typedef struct QMsgUIChannelInfo_t
{
    uint32_t team_id;
    uint32_t channel_id;
    char *channel_name;
} QMsgUIChannelInfo_t;

typedef struct QMsgUISendASCIIMsg_t
{
    uint32_t team_id;
    uint32_t channel_id;
    char *message;
} QMsgUISendASCIIMsg_t;

typedef struct QMsgUIReceiveASCIIMessage_t
{
    uint32_t team_id;
    uint32_t channel_id;
    uint64_t time;
    char *message;
} QMsgUIReceiveASCIIMessage_t;

typedef struct QMsgUIWatch_t
{
    uint32_t team_id;
    uint32_t channel_id;
} QMsgUIWatch_t;

typedef struct QMsgUIUnwatch_t
{
    uint32_t team_id;
    uint32_t channel_id;
} QMsgUIUnwatch_t;

typedef struct QMsgUIRequestMessages_t
{
    uint32_t team_id;
    uint32_t channel_id;
    uint64_t before_time;
    uint16_t message_count;
} QMsgUIRequestMessages_t;

// The following union will hold the contents of a single UI message
typedef struct QMsgUIMessage
{
    QMsgUIMessageType type;
    union
    {
        QMsgUIUnlock_t unlock;
        QMsgUIIsLocked_t is_locked;
        QMsgUIDeviceInfo_t device_info;
        QMsgUIGetTeams_t get_teams;
        QMsgUITeamInfo_t teams_info;
        QMsgUIGetChannels_t get_channels;
        QMsgUIChannelInfo_t channel_info;
        QMsgUISendASCIIMsg_t send_ascii_message;
        QMsgUIReceiveASCIIMessage_t receive_ascii_message;
        QMsgUIWatch_t watch;
        QMsgUIUnwatch_t unwatch;
        QMsgUIRequestMessages_t request_messages;
    } u;
} QMsgUIMessage;

#ifdef __cplusplus
} // extern "C"
#endif
