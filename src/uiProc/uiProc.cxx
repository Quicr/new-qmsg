
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <iostream>

#include "qmsg/encoder.h"

#include "Parser.hh"
#include "Keyboard.hh"
#include "Sender.hh"
#include "Receiver.hh"

using namespace std;

int main(int argc, char *argv[])
{

    fprintf(stderr, "UI: Starting\n");

    int keyboard_fd = 0;

    // int sec2uiFD = open("/tmp/pipe-s2u", O_RDONLY, O_NONBLOCK);
    int sec2uiFD = 0 ;
    // assert(sec2uiFD >= 0);
    fprintf(stderr, "UI: Got pipe from secProc\n");

    // int ui2secFD = open("/tmp/pipe-u2s", O_WRONLY, O_NONBLOCK);
    int ui2secFD = 0;
    // assert(ui2secFD >= 0);
    fprintf(stderr, "UI: Got pipe to secProc\n");

    const int Buffer_Size = 1024;
    char incoming_buffer[Buffer_Size];
    char keyboard_buffer[Buffer_Size];

    Parser parser;
    Parser::Command command;
    Keyboard keyboard;
    Sender sender(ui2secFD);
    Receiver receiver(sec2uiFD);
    unsigned int buffer_length;

    while (true)
    {
        buffer_length = 0;
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        fd_set fdSet;
        int maxFD = 0;
        FD_ZERO(&fdSet);
        FD_SET(keyboard_fd, &fdSet);
        maxFD = (keyboard_fd > maxFD) ? keyboard_fd : maxFD;
        FD_SET(sec2uiFD, &fdSet);
        maxFD = (sec2uiFD > maxFD) ? sec2uiFD : maxFD;
        int selected_fd = select(maxFD+1, &fdSet, NULL, NULL, &timeout);
        assert(selected_fd >= 0);
        // Process the keyboard
        keyboard.SetFd(keyboard_fd);
        if (keyboard.HasMessage(selected_fd, fdSet))
        {
            keyboard.Read(keyboard_buffer, buffer_length, Buffer_Size);

            if (buffer_length > 0)
            {
                // Print the message from the keyboard
                fprintf(stderr, "UI: Read %d bytes from keyboard: ", buffer_length);
                fwrite(keyboard_buffer, 1, buffer_length, stderr);
                fprintf(stderr, "\n");

                // Parse the input for a command such as help
                bool res = parser.Parse(keyboard_buffer, buffer_length, command);

                // Handle the parsed message
                if (res == false)
                    continue;

                if (command == Parser::Command::help)
                {
                    // Print the help command
                    fprintf(stderr, "UI: Help command received");
                    continue;
                }

                // Send to secure processor
                const char *sending = "sending: ";
                sender.SendMessage(keyboard_buffer, buffer_length);
            }
        }

        // processSecureProc
        // if ((selected_fd > 0) && (FD_ISSET(sec2uiFD, &fdSet)))
        if (receiver.HasMessage(selected_fd, fdSet))
        {
            // fprintf(stderr, "UI: Reding Sec Proc\n");
            // ssize_t num = read(sec2uiFD, incoming_buffer, Buffer_Size);
            receiver.ReceiveMessage(incoming_buffer, buffer_length, Buffer_Size);
            if (buffer_length > 0)
            {
                fprintf(stderr, "UI: Read %d bytes from SecProc: ", buffer_length);
                fwrite(incoming_buffer, 1, buffer_length, stderr);
                fprintf(stderr, "\n");
            }
        }
    }

    return 0;
}
