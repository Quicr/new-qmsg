#include "Receiver.hh"
#include <unistd.h>

Receiver::Receiver(int sec_to_ui_fd) : sec_to_ui_fd(sec_to_ui_fd)
{
}

Receiver::~Receiver()
{
}

bool Receiver::HasMessage(int selected_fd, fd_set &fdSet)
{
    return false;
}

void Receiver::ReceiveMessage(char *buffer, unsigned int &buffer_length, const unsigned int buffer_size)
{
    buffer_length = read(sec_to_ui_fd, buffer, buffer_size);

    // decode message
}

void Receiver::DecodeMessage()
{
}