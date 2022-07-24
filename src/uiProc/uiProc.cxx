#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include "UserInterface.hh"

constexpr int Buffer_Size = 1024;

int main(int argc, char *argv[])
{

    fprintf(stderr, "UI: Starting\n");

    // Keyboard file descriptor
    int keyboard_fd = 0;

    // Setup the security to ui file descriptor
    int sec_to_ui_fd = open("/tmp/pipe-s2u", O_RDONLY, O_NONBLOCK);
    assert(sec_to_ui_fd >= 0);
    fprintf(stderr, "UI: Got pipe from secProc\n");

    // Setup the ui to security file descriptor
    int ui_to_sec_fd = open("/tmp/pipe-u2s", O_WRONLY, O_NONBLOCK);
    assert(ui_to_sec_fd >= 0);
    fprintf(stderr, "UI: Got pipe to secProc\n");

    /*
    * NOTE - If you want to skip having the sec process and net process
    * running then comment out the above lines up until line 19.
    * As well comment out line 46 in UserInterface.cxx
    * the HandleReceiver() invocation.
    *
    * TODO This is only here because the slow net and slow sec do not play
    * well together which made it impossible to work with so I needed to add
    * this in as a hack.
    */
    // int sec_to_ui_fd = 1;
    // int ui_to_sec_fd = 1;

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    fd_set fdSet;
    int maxFD = 0;

    int selected_fd = 0;

    UserInterface user_interface(
        keyboard_fd, sec_to_ui_fd, ui_to_sec_fd, Buffer_Size);
    user_interface.Start();
    while (user_interface.Running())
    {
        FD_ZERO(&fdSet);
        FD_SET(keyboard_fd, &fdSet);
        maxFD = (keyboard_fd > maxFD) ? keyboard_fd : maxFD;
        FD_SET(sec_to_ui_fd, &fdSet);
        maxFD = (sec_to_ui_fd > maxFD) ? sec_to_ui_fd : maxFD;
        selected_fd = select(maxFD+1, &fdSet, NULL, NULL, &timeout);
        assert(selected_fd >= 0);

        // Process the user interface
        user_interface.Process(selected_fd, fdSet);
    }

    return 0;
}
