#pragma once
#include "qmsg/encoder.h"
#include "qmsg/net_types.h"

#include <functional>
#include <quicr/quicr_client.h>

enum struct LoopProcessResult {
    SUCCESS = 0,
    INVALID_ARGS,
    ENCODER_ERROR

};

enum struct EventSource {
    SecProc = 0,
    Network = 1
};

/// Message Process Loop with encode and decode and usually loopy stuff
struct MessageLoop {

    LoopProcessResult process(uint16_t read_buffer_size_in);
    QMsgEncoderResult decode();

    std::function<bool (QMsgNetMessage&, EventSource, quicr::bytes&& )> process_net_message_fn = nullptr;
    // generic loop fn to do other things than QMesg Parsing
    std::function<void ()> loop_fn = nullptr;


    bool keep_processing = true;
    int read_from_fd = -1;
    const uint16_t read_buffer_size = 8192;
};