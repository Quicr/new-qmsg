#pragma once

#include <stdio.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/select.h>

// Read and parse input from keybaord or similar user entry system
class FdReader
{
public:
    FdReader(int fd, unsigned long buffer_size);
    ~FdReader();

    bool HasMessage(const int selected_fd, fd_set &fdSet);
    char* Read(unsigned long offset = 0);
    void SlideBuffer(unsigned long offset);
    char* Data();
    unsigned int BufferLength();
    unsigned int BufferSize();

    void Flush();
private:
    int fd;
    unsigned long buffer_size;
    unsigned long buffer_length;
    char* buffer_data;
};