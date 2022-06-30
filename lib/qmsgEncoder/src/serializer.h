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

        // UI<=>Sec Interface
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgUISendASCIIMessage_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgUIReceiveASCIIMessage_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgUIWatchChannel_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgUIUnwatchChannel_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgUIUnlock_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgUIIsLocked_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgUIMLSSignatureHash_t &message);

        // NetI<=>Sec Interface
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgNetSendASCIIMessage_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgNetReceiveASCIIMessage_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgNetWatchDevices_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgNetUnwatchDevices_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgNetMLSSignatureHash_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgNetMLSKeyPackage_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgNetMLSAddKeyPackage_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgNetMLSWelcome_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgNetMLSCommit_t &message);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgNetDeviceInfo_t &message);

    protected:
        std::size_t Serialize(DataBuffer &data_buffer, std::uint16_t value);
        std::size_t Serialize(DataBuffer &data_buffer, std::uint32_t value);
        std::size_t Serialize(DataBuffer &data_buffer, std::uint64_t value);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgOpaque_t &value);
        std::size_t Serialize(DataBuffer &data_buffer,
                              const QMsgDeviceList_t &value);

        DataBuffer null_buffer;
};

} // namespace qmsg
