#include "FdReader.hh"

#include <stdlib.h>
#include <string.h>

FdReader::FdReader(int fd, unsigned long buffer_size) :
    fd(fd), buffer_size(buffer_size), buffer_length(0)
{
    buffer_data = new char[buffer_size];
}

FdReader::~FdReader()
{
    delete [] buffer_data;
}

bool FdReader::HasMessage(const int selected_fd, fd_set &fdSet)
{
    return (selected_fd > 0) && (FD_ISSET(fd, &fdSet));
}

char* FdReader::Read(unsigned long offset)
{
    buffer_length = read(fd,
                         buffer_data + offset,
                         buffer_size - offset);

    buffer_length += offset;
    buffer_data[strcspn(buffer_data, "\n")] = 0;
    return buffer_data;
}

void FdReader::SlideBuffer(unsigned long offset)
{
    memmove(buffer_data, buffer_data + offset, buffer_length - offset);
}

char* FdReader::Data()
{
    return buffer_data;
}

unsigned int FdReader::BufferLength()
{
    return buffer_length;
}

unsigned int FdReader::BufferSize()
{
    return buffer_size;
}

void FdReader::Flush()
{
    buffer_length = 0;
}
