#include <unistd.h>
#include <stdio.h>
#include <sys/uio.h>

class Receiver
{
public:
    Receiver(const int sec_to_ui_fd);
    ~Receiver();

    bool HasMessage(int selected_fd, fd_set &fdSet);
    void ReceiveMessage(char *buffer, unsigned int &buffer_length, const unsigned int buffer_size);
private:
    void DecodeMessage();

    int sec_to_ui_fd;
};