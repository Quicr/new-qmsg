/*
 *  deserializer.h
 *
 *  Copyright (C) 2022
 *  Cisco Systems, Inc.
 *  All Rights Reserved
 *
 *  Description:
 *      This file defines the QMsgDeserializer, which will deserialize a QMsg
 *      data structure from the given buffer.
 *
 *  Portability Issues:
 *      None.
 */

#include <vector>
#include "qmsg/encoder.h"
#include "data_buffer.h"

namespace qmsg
{

// Class to perform deserialization of data structures
class QMsgDeserializer
{
    public:
        QMsgDeserializer() = default;
        ~QMsgDeserializer()
        {
            FreeAllocations();
        }

        std::size_t DeserializeUIMessageType(DataBuffer &data_buffer,
                                             QMsgUIMessageType &type);
        std::size_t DeserializeMessageLength(DataBuffer &data_buffer,
                                             std::uint32_t &message_length);

        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgUIUnlock_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgUIIsLocked_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgUIDeviceInfo_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgUIGetTeams_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgUITeamInfo_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgUIGetChannels_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgUIChannelInfo_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgUISendASCIIMsg_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgUIReceiveASCIIMessage_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgUIWatch_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgUIUnwatch_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgUIRequestMessages_t &message);

    protected:
        std::size_t Deserialize(DataBuffer &data_buffer, std::uint16_t &value);
        std::size_t Deserialize(DataBuffer &data_buffer, std::uint32_t &value);
        std::size_t Deserialize(DataBuffer &data_buffer, std::uint64_t &value);
        std::size_t Deserialize(DataBuffer &data_buffer, char *&value);
        void FreeAllocations();

        std::vector<std::uint8_t *> allocations;
};

} // namespace
