#pragma once

#include <time.h>
#include <sys/time.h>
#include <vector>
#include <string>

#include "KeyBoardReader.hh"
#include "Sender.hh"
#include "Channel.hh"
#include "Parser.hh"
#include "Team.hh"
#include "qmsg/encoder.h"

enum Command
{
    help = 0,
    info,
    set, // set command for a username
    connect,
    join,
    leave,
    direct,
    bye
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
    bool isValidChannel(std::string channelExists);
private:
    tm* GetCurrentSystemTime();
    void HandleKeyboard(int selected_fd, fd_set fdSet);
    void HandleReceiver(int selected_fd, fd_set fdSet);
    void Parse();
    void PrintMessage(const char* msg);
    void PrintTimestampedMessage(const char* msg);
    void JoinTeam(const std::string team);

    const char* commands[8] =
    {
        "/help",
        "/info",
        "/set",
        "/connect",
        "/join",
        "/leave",
        "/direct",
        "/bye"
    };

    const char* help_desc[9] = {
            "Next steps set your user name by entering /set username <xyz>\n",
            "Connect to your team by entering /connect <team name> <chat name>\n",
            "Once connected you'll be automatically added to all channels\n",
            "Join a chat by entering /join <chat name>\n",
            "Leave a chat by entering /leave <chat name>\n",
            "Access help at any time by entering /help\n",
            "Access info at any time by entering /info\n",
            "Message a user directly by entering /direct <username>\n",
            "Enter /bye to exit the Cisco Secure Messaging app\n"
    };

    const std::string bye_str = "Thanks for joining Cisco Secure Messaging\n";

    KeyBoardReader* keyboard;
    KeyBoardReader* receiver;
    Sender* sender;
    Parser* parser;
    std::vector<Channel> joined_channels;
    std::vector<Channel> all_channels;

    int selected_fd;
    bool is_running;
    char* username;

    // TODO move into a new a new class from keyboard (sender)

    // TODO move these into a new class from security (receiver)
    QMsgEncoderResult qmsg_enc_sec_res;
    QMsgEncoderContext *sec_context;
    QMsgNetMessage sec_message;
    unsigned long sec_fragment_size = 0;
    unsigned long sec_total_consumed = 0;
    unsigned long sec_consumed = 0;
};