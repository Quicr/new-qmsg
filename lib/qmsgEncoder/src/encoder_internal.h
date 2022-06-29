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

} // namespace qmsg
