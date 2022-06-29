#pragma once

#include <stdio.h>
#include <sys/uio.h>
#include <unistd.h>

struct MsgBuffer
{
    char* data;
    unsigned int size;
    unsigned int length;
};

class FdReader
{
public:
    FdReader(int fd, unsigned int buffer_size);
    ~FdReader();

    bool HasMessage(const int selected_fd, fd_set &fdSet);
    MsgBuffer& Read();
    MsgBuffer& Buffer();
    char* Data();
    unsigned int BufferLength();
    unsigned int BufferSize();
    void Flush();
private:
    int fd;
    MsgBuffer buffer;
};