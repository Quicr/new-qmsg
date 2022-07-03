#include "KeyBoardReader.hh"

#include <stdlib.h>
#include <string.h>

KeyBoardReader::KeyBoardReader(int fd_in, unsigned long buffer_size_in) :
    fd(fd_in), buffer_size(buffer_size_in), buffer_length(0)
{
    buffer_data = new char[buffer_size];
}

KeyBoardReader::~KeyBoardReader()
{
    delete [] buffer_data;
}

bool KeyBoardReader::HasMessage(const int selected_fd, fd_set &fdSet)
{
    return (selected_fd > 0) && (FD_ISSET(fd, &fdSet));
}

char* KeyBoardReader::Read(unsigned long offset)
{
    buffer_length = read(fd,
                         buffer_data + offset,
                         buffer_size - offset);

    buffer_length += offset;
    buffer_data[strcspn(buffer_data, "\n")] = 0;
    return buffer_data;
}

void KeyBoardReader::SlideBuffer(unsigned long offset)
{
    memmove(buffer_data, buffer_data + offset, buffer_length - offset);
}

char* KeyBoardReader::Data()
{
    return buffer_data;
}

unsigned int KeyBoardReader::BufferLength()
{
    return buffer_length;
}

unsigned int KeyBoardReader::BufferSize()
{
    return buffer_size;
}

void KeyBoardReader::Flush()
{
    buffer_length = 0;
}