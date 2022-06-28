/*
 *  serializer.h
 *
 *  Copyright (C) 2022
 *  Cisco Systems, Inc.
 *  All Rights Reserved
 *
 *  Description:
 *      This file defines the QMsgSerializer, which will serialize a QMsg
 *      data structure into the given buffer.
 *
 *  Portability Issues:
 *      None.
 */

#include "qmsg/encoder.h"
#include "data_buffer.h"

namespace qmsg
{

// Class to perform serialization of data structures
class QMsgSerializer
{
    public:
        QMsgSerializer() = default;
        ~QMsgSerializer() = default;

        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgUIUnlock_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgUIIsLocked_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgUIDeviceInfo_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgUIGetTeams_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgUITeamInfo_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgUIGetChannels_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgUIChannelInfo_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgUISendASCIIMsg_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgUIReceiveASCIIMessage_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgUIWatch_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgUIUnwatch_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgUIRequestMessages_t &message);

    protected:
        std::size_t Serialize(DataBuffer &data_buffer, std::uint16_t value);
        std::size_t Serialize(DataBuffer &data_buffer, std::uint32_t value);
        std::size_t Serialize(DataBuffer &data_buffer, std::uint64_t value);
        std::size_t Serialize(DataBuffer &data_buffer, char *value);

        DataBuffer null_buffer;
};

} // namespace qmsg
