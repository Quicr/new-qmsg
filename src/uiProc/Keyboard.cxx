#include "Keyboard.hh"

bool Keyboard::HasMessage(const int selected_fd, fd_set &fdSet)
{
    return (selected_fd > 0) && (FD_ISSET(keyboard_fd, &fdSet));
}

void Keyboard::Read(char* buffer, unsigned int &buffer_length, const unsigned int buffer_size)
{
    buffer_length = read(keyboard_fd, buffer, buffer_size);
}

void Keyboard::SetFd(const int keyboard_fd)
{
    this->keyboard_fd = keyboard_fd;
}