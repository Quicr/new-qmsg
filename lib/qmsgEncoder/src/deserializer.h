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
#include <stdexcept>
#include "qmsg/encoder.h"
#include "qmsg/data_buffer.h"

namespace qmsg
{

/*
 * QMsgDeserializerException
 *
 * This class defines an exception object that will be thrown if the
 * QMsgDeserializer encounters an error when deserializing data.  This should
 * only happen if an invalid or corrupt message is discovered.
 */
class QMsgDeserializerException : public std::runtime_error
{
    public:
        explicit QMsgDeserializerException(const std::string &what_arg) :
            std::runtime_error(what_arg)
        {
        }

        explicit QMsgDeserializerException(const char *what_arg) :
            std::runtime_error(what_arg)
        {
        }
};

// Class to perform deserialization of data structures
class QMsgDeserializer
{
    public:
        QMsgDeserializer() = default;
        ~QMsgDeserializer()
        {
            FreeAllocations();
        }

        std::size_t DeserializeMessageLength(DataBuffer &data_buffer,
                                             std::uint32_t &message_length);

        // UI<=>Sec Interface
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgUIMessageType &type);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgUISendASCIIMessage_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgUIReceiveASCIIMessage_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgUIWatchChannel_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgUIUnwatchChannel_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgUIUnlock_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgUIIsLocked_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgUIMLSSignatureHash_t &message);

        // Net<=>Sec Interface
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgNetMessageType &type);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgNetSendASCIIMessage_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgNetReceiveASCIIMessage_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgNetWatchDevices_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgNetUnwatchDevices_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgNetMLSSignatureHash_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgNetMLSKeyPackage_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgNetMLSAddKeyPackage_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgNetMLSWelcome_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgNetMLSCommit_t &message);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgNetDeviceInfo_t &message);

    protected:
        std::size_t Deserialize(DataBuffer &data_buffer, std::uint16_t &value);
        std::size_t Deserialize(DataBuffer &data_buffer, std::uint32_t &value);
        std::size_t Deserialize(DataBuffer &data_buffer, std::uint64_t &value);
        std::size_t Deserialize(DataBuffer &data_buffer, QMsgOpaque_t &value);
        std::size_t Deserialize(DataBuffer &data_buffer,
                                QMsgDeviceList_t &value);
        void FreeAllocations();

        std::vector<std::uint8_t *> allocations;
};

} // namespace
