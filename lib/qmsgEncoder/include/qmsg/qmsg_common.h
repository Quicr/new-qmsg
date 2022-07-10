/*
 *  qmsg_connon.cpp
 *
 *  Copyright (C) 2022
 *  Cisco Systems, Inc.
 *  All Rights Reserved
 *
 *  Description:
 *      This file defines common data types used in both the UI<=>Sec and
 *      Sec<=>Net interfaces.
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef uint32_t QMsgMessageType;               // Message type (wire encoding)
typedef uint32_t QMsgLength;                    // Size of length fields
typedef uint32_t QMsgOrgID;                     // Organization ID
typedef uint32_t QMsgTeamID;                    // Team ID
typedef uint32_t QMsgChannelID;                 // Channel ID
typedef uint32_t QMsgDeviceID;                  // Device ID
typedef uint32_t QMsgMessageID;                 // Message ID
typedef uint32_t QMsgPin;                       // PIN value

typedef struct QMsgOpaque_t                     // Opaque type
{
    QMsgLength length;
    uint8_t *data;
} QMsgOpaque_t;

typedef struct QMsgDeviceList_t                 // Device list type
{
    size_t num_devices;
    QMsgDeviceID *device_list;
} QMsgDeviceList_t;

#ifdef __cplusplus
} // extern "C"
#endif
