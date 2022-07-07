#pragma once

#include <stdio.h>
#include <sys/uio.h>
#include <unistd.h>

// Read and parse input from keybaord or similar user entry system
class KeyBoardReader
{
public:
    KeyBoardReader(int fd, unsigned long buffer_size);
    ~KeyBoardReader();

    bool HasMessage(const int selected_fd, fd_set &fdSet);
    char* Read(unsigned long offset = 0);
    void SlideBuffer(unsigned long offset);
    char* Data();
    unsigned int BufferLength();
    unsigned int BufferSize();

    void Flush();
private:
    // read and parse messages
    void message_loop();

    int fd;
    unsigned long buffer_size;
    unsigned long buffer_length;
    char* buffer_data;
};