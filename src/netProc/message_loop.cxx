#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "message_loop.h"

LoopProcessResult MessageLoop::process(uint16_t read_buffer_size_in)
{
    if (read_from_fd == -1) {
        return  LoopProcessResult::INVALID_ARGS;
    }

    uint16_t buffer_size = read_buffer_size_in;
    if (buffer_size == 0 || buffer_size >  8192) {
        buffer_size = read_buffer_size;
    }

    char* read_buffer = (char*) malloc(buffer_size);

    size_t fragment_size = 0;
    QMsgEncoderResult qmsg_enc_result;
    QMsgEncoderContext *context;
    size_t consumed = 0;
    size_t total_consumed = 0;
    QMsgNetMessage message = {};

    if (QMsgEncoderInit(&context))
    {
        // log an error and...
        return LoopProcessResult::ENCODER_ERROR;
    }

    while(keep_processing)
    {
        // waitForInput
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        fd_set fdSet;
        int maxFD=0;

        FD_ZERO(&fdSet);
        FD_SET(read_from_fd, &fdSet);
        maxFD = (read_from_fd > maxFD) ? read_from_fd : maxFD;

        int numSelectFD = select( maxFD + 1 , &fdSet , nullptr, nullptr, &timeout );
        assert( numSelectFD >= 0 );

        // process messages
        if ((read_from_fd > 0) && (FD_ISSET(read_from_fd, &fdSet)))
        {
            ssize_t num = read(read_from_fd,
                               read_buffer + fragment_size,
                               buffer_size - fragment_size);

            // Update the total bytes to include the previous fragment
            num += fragment_size;

            // Reset the fragment length
            fragment_size = 0;

            // Set the total consumed to 0
            total_consumed = 0;

            // Process as many messages in the buffer as possible
            do
            {
                consumed = 0;
                qmsg_enc_result = QMsgNetDecodeMessage(context,
                                                       read_buffer + total_consumed,
                                                       num - total_consumed,
                                                       &message,
                                                       &consumed);

                // Update the total octets consumed
                total_consumed += consumed;

                if (qmsg_enc_result == QMsgEncoderSuccess)
                {
                    if(process_net_message_fn != nullptr) {
                        bool result = process_net_message_fn(message);
                        // log the result
                        continue;
                    }
                }

                if ((qmsg_enc_result == QMsgEncoderInvalidMessage) ||
                    (qmsg_enc_result == QMsgEncoderCorruptMessage))
                {
                    /// Just log the fact the message was invalid or corrupu; it will get skipped over
                }
            } while ((total_consumed < num) && (consumed > 0));

            // If there is partial data in the buffer, move it to the front
            // and take note of the new fragment size
            if (total_consumed < num)
            {
                memmove(read_buffer, read_buffer + total_consumed, num - total_consumed);
                fragment_size = num - total_consumed;
            }

        }
    }

    QMsgEncoderDeinit(context);
    return LoopProcessResult::SUCCESS;
}