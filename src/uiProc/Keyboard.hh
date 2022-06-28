#include <stdio.h>
#include <sys/uio.h>
#include <unistd.h>

class Keyboard
{
public:
    bool HasMessage(const int selected_fd, fd_set &fdSet);
    void Read(char* buffer, unsigned int &buffer_length, const unsigned int buffer_size);
    void SetFd(const int keyboard_fd);
private:
    int keyboard_fd;
};