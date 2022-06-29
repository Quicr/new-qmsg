#include "FdReader.hh"

#include <stdlib.h>

FdReader::FdReader(int fd, unsigned int buffer_size) :
    fd(fd)
{
    buffer.data = new char[buffer_size];
    buffer.size = buffer_size;
    buffer.length = 0;
}

FdReader::~FdReader()
{
    delete [] buffer.data;
}

bool FdReader::HasMessage(const int selected_fd, fd_set &fdSet)
{
    return (selected_fd > 0) && (FD_ISSET(fd, &fdSet));
}

MsgBuffer& FdReader::Read()
{
    buffer.length = read(fd, buffer.data, buffer.size);

    return buffer;
}

MsgBuffer& FdReader::Buffer()
{
    return buffer;
}

char* FdReader::Data()
{
    return buffer.data;
}

unsigned int FdReader::BufferLength()
{
    return buffer.length;
}

unsigned int FdReader::BufferSize()
{
    return buffer.size;
}

void FdReader::Flush()
{
    free(buffer.data);
}