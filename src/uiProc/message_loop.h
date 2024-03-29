#pragma once
#include "qmsg/encoder.h"
#include "qmsg/ui_types.h"

#include <functional>

enum struct LoopProcessResult {
    SUCCESS = 0,
    INVALID_ARGS,
    ENCODER_ERROR

};


/// Message Process Loop with encode and decode and usually loopy stuff
struct MessageLoop {

    LoopProcessResult process(uint16_t read_buffer_size_in);
    QMsgEncoderResult decode();

    std::function<bool (QMsgUIMessage&)> process_sec_message_fn = nullptr;
    // generic loop fn to do other things than QMesg Parsing
    std::function<void ()> loop_fn = nullptr;

    bool keep_processing = true;
    int read_from_fd = -1;
    const uint16_t read_buffer_size = 8192;
};