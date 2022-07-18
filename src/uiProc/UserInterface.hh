#pragma once

#include <time.h>
#include <sys/time.h>
#include <vector>
#include <string>
#include <queue>

#include "qmsg/encoder.h"

#include "Channel.hh"
#include "FdReader.hh"
#include "Parser.hh"
#include "Profile.hh"
#include "Sender.hh"
#include "Team.hh"
#include <stdio.h>
#include <iostream>
#include <string>
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

    void Process(int selected_fd, fd_set fdSet);
    bool Running();
    void Start();
    void Stop();

private:
    void DisplayHelpMessage();
    void Draw();
    void GetUserPin();
    tm* GetCurrentSystemTime();
    void HandleKeyboard(int selected_fd, fd_set fdSet);
    void HandleReceiver(int selected_fd, fd_set fdSet);
    void HandleUserInput();
    bool IsValidChannel(std::string channelExists);
    void JoinTeam(const std::string team);
    void Parse();
    void PrintMessage(const char* msg);
    void PrintTimestampedMessage(const char* msg);

    void BuildMessageUIMatrix();
    std::vector<std::string> BuildMessage(std::string user_name, std::string message);

    // HACK these should be calculated based on the window size
    // Assumed to be full screened window on a 1920x1080 screen.
    const int num_channel_spaces = 24;
    // const int num_messages_spaces = 100;
    const int num_messages_spaces = 100;
    const int num_user_spaces = 24;

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

    FdReader* keyboard;
    FdReader* receiver;
    Sender* sender;
    Parser* parser;
    Profile* profile;
    std::vector<Channel> joined_channels;
    std::vector<Channel> all_channels;

    int selected_fd;
    bool is_running;

    // TODO move into a new a new class from keyboard (sender)

    // TODO move these into a new class from security (receiver)
    QMsgEncoderResult qmsg_enc_sec_res;
    QMsgEncoderContext *sec_context;
    QMsgNetMessage sec_message;
    unsigned long sec_fragment_size = 0;
    unsigned long sec_total_consumed = 0;
    unsigned long sec_consumed = 0;


    struct FdMessage
    {
        size_t length;
        char* data;
    };
    std::queue<FdMessage> outgoing_queue;
    std::queue<FdMessage> incoming_queue;
    std::vector<std::string> messages;

    bool update_draw;

    std::vector<std::string> draw_matrix;
};