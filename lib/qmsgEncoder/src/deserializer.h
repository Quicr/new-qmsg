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

        std::size_t Deserialize(DataBuffer &buffer,
                                const QMsgUIUnlock_t &unlock);

    protected:
        // Free any memory allocated by the deserializer
        void FreeAllocations()
        {
            if (allocations.empty()) return;

            for (std::size_t i = 0; i < allocations.size(); i++)
            {
                free(allocations[i]);
            }

            allocations.clear();
        }

        std::vector<std::uint8_t *> allocations;
};

} // namespace
