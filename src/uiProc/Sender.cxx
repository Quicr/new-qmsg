#include "Sender.hh"

Sender::Sender(int ui_to_sec_fd) : ui_to_sec_fd(ui_to_sec_fd)
{
}

Sender::~Sender()
{
}

void Sender::SendMessage(char *buffer, const unsigned int buffer_length)
{
    // Write the buffer to the fd
    fprintf(stderr, "Sending: ", buffer_length);
    write(ui_to_sec_fd, buffer, buffer_length);
    fprintf(stderr, "\n");
}

void Sender::EncodeMessage(char *buffer, const unsigned int buffer_length)
{
}