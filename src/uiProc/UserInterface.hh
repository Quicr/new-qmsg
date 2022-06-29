#pragma once

#include <time.h>
#include <sys/time.h>

#include "FdReader.hh"
#include "Sender.hh"
#include "Channel.hh"
#include "Parser.hh"

enum Command
{
    help,
    info,
    set, // set command for a username
    connect,
    join,
    leave,
    direct,
};

class UserInterface
{
public:
    UserInterface(const int keyboard_fd,
                  const int sec_to_ui_fd,
                  const int ui_to_sec_id,
                  const unsigned int buffer_size);

    ~UserInterface();

    void Start();
    void Process(int selected_fd, fd_set fdSet);
    bool Running();
    void Stop();

    void DisplayHelpMessage();
private:
    tm* GetCurrentSystemTime();
    void HandleKeyboard(int selected_fd, fd_set fdSet);
    void HandleReceiver(int selected_fd, fd_set fdSet);
    void Parse();
    void PrintMessage(const char* msg);
    void PrintTimestampedMessage(const char* msg);

    const char* commands[7] =
    {
        "/help",
        "/info",
        "/set",
        "/connect",
        "/join",
        "/leave",
        "/direct"
    };

    FdReader* keyboard;
    FdReader* receiver;
    Sender* sender;
    Parser* parser;
    // Channel channel;
    int selected_fd;
    bool is_running;
    char* username;
};