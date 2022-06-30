#pragma once

#include <stdio.h>
#include <unistd.h>

#include "qmsg/encoder.h"

class Sender
{
public:
    Sender(int ui_to_sec_fd);
    ~Sender();

    bool HasMessage(int selected_fd);
    void Read(char* buffer, unsigned int &buffer_length, const unsigned int buffer_size);
    void ReadKeyboard();
    void SendMessage(char *buffer, const unsigned long buffer_length);
private:
    void EncodeMessage(char *buffer, const unsigned int buffer_length);

    int ui_to_sec_fd;
    QMsgEncoderContext *context;
};