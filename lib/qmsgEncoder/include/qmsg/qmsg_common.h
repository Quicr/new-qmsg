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

typedef uint32_t QMsgTeamID;
typedef uint32_t QMsgChannelID;
typedef uint16_t QMsgDeviceID;
typedef uint32_t QMsgPin;

typedef struct QMsgOpaque_t
{
    uint32_t length;
    uint8_t *data;
} QMsgOpaque_t;

typedef struct QMsgDeviceList_t
{
    uint32_t num_devices;
    QMsgDeviceID *device_list;
} QMsgDeviceList_t;

#ifdef __cplusplus
} // extern "C"
#endif
