#include "UserInterface.hh"
#include <string.h>
#include <unistd.h>
#include <stdio.h>


UserInterface::UserInterface(const int keyboard_fd,
                             const int sec_to_ui_fd,
                             const int ui_to_sec_id,
                             const unsigned int buffer_size) :
    selected_fd(0),
    is_running(false)
{
    keyboard = new FdReader(keyboard_fd, buffer_size);
    receiver = new FdReader(sec_to_ui_fd, buffer_size);
    sender = new Sender(ui_to_sec_id);
    parser = new Parser();
}

UserInterface::~UserInterface()
{
    delete keyboard;
    delete receiver;
    delete sender;
    delete parser;
    delete username;
    delete context;
}

void UserInterface::Start()
{
    PrintMessage("Welcome to Cisco Secure Messaging\n\n");
    PrintTimestampedMessage("Next steps set your user name by entering /set username <xyz>\n");
    PrintTimestampedMessage("Connect to your team by entering /connect <team>\n");
    PrintTimestampedMessage("Once connected you'll be automatically added to all channels\n");
    PrintTimestampedMessage("Join a chat by entering /join <chat name>\n");
    PrintTimestampedMessage("Leave a chat by entering /leave <chat name>\n");
    PrintTimestampedMessage("Access help at any time by entering /help\n");
    PrintTimestampedMessage("Access info at any time by entering /info\n");
    PrintTimestampedMessage("Message a user directly by entering /direct <username>\n");
    PrintTimestampedMessage("Thanks for joining Cisco Secure Messaging\n");
    is_running = true;
}

void UserInterface::Process(int selected_fd, fd_set fdSet)
{
    if (is_running)
    {
        HandleKeyboard(selected_fd, fdSet);
        HandleReceiver(selected_fd, fdSet);
    }
}

bool UserInterface::Running()
{
    return is_running;
}

void UserInterface::Stop()
{
    is_running = false;
    // TODO
    //  Display the closing message of the UI
}

void UserInterface::DisplayHelpMessage()
{
}

tm *UserInterface::GetCurrentSystemTime()
{
    time_t raw_time;
    struct tm *timeinfo;

    time(&raw_time);
    timeinfo = localtime(&raw_time);
    return timeinfo;
}

void UserInterface::HandleKeyboard(int selected_fd, fd_set fdSet)
{
    if (keyboard->HasMessage(selected_fd, fdSet))
    {
        keyboard->Read();
        if (keyboard->BufferLength() > 0)
        {
            Command command;

            // TODO remove
            fprintf(stderr, "UI: Read %d bytes from keyboard: ", keyboard->BufferLength());
            fwrite(keyboard->Data(), 1, keyboard->BufferLength(), stderr);
            fprintf(stderr, "\n");

            // Parse the input for a command such as help
            // bool res = parser->Parse(keyboard->Data(), keyboard->BufferLength(), command);

            // parse the input for commands
            if (keyboard->Data()[0] == '/')
            {

                char* command_token = strtok((keyboard->Data()), " ");
                fprintf(stderr, "UI: command received - %s\n", command_token);
                if (strcmp(command_token, commands[Command::help]) == 0)
                {
                    // TODO Print help
                    DisplayHelpMessage();
                }
                else if (strcmp(command_token, commands[Command::info]) == 0)
                {
                    // TODO
                }
                else if (strcmp(command_token, commands[Command::set]) == 0)
                {
                    char* set_token = strtok(NULL, " ");
                    if (set_token)
                    {
                        char* argument_token = strtok(NULL, " ");
                        if (argument_token)
                        {
                            fprintf(stderr, "Username has been set to %s",
                                argument_token);
                        }
                        else
                        {
                            // TODO error
                        }
                    }
                    else
                    {
                        // TODO error missing argument
                    }
                }
                else if (strcmp(command_token, commands[Command::connect]) == 0)
                {
                    // Connect to a team room
                }
                else if (strcmp(command_token, commands[Command::join]) == 0)
                {
                    // Join a channel
                    std::string join_token(strtok(NULL, " "));
                    fprintf(stderr, "Connecting to a room %s", join_token);

                    if (join_token.length() > 0)
                    {
                        bool channel_exists = false;
                        unsigned int idx = 0;
                        for (unsigned int i = 0; i < all_channels.size(); i++)
                        {
                            if (join_token == all_channels[i].Name())
                            {
                                channel_exists = true;
                                idx = i;
                                break;
                            }
                        }

                        if (channel_exists)
                        {
                            // Send a channel join request
                            sender->SendMessage(all_channels[idx].Name().c_str(), join_token.length());

                            // Keep track of the channels we've joined
                            joined_channels.push_back(all_channels[idx]);
                        }
                    }
                    else
                    {
                        // TODO error
                    }
                }
                else if (strcmp(command_token, commands[Command::leave]) == 0)
                {
                    // TODO
                }
                else if (strcmp(command_token, commands[Command::direct]) == 0)
                {
                    // TODO
                }
                else
                {
                    // error
                }

                // keyboard->Flush();
                return;
            }
            else
            {
                if (false)
                {
                    // TODO if we are not in a chat room then
                    // normal messages do nothing.
                    // and note we are not in a channel
                }


                // Send to secure process
                sender->SendMessage(keyboard->Data(), keyboard->BufferLength());
            }
        }
    }
}

void UserInterface::HandleReceiver(int selected_fd, fd_set fdSet)
{
    if (receiver->HasMessage(selected_fd, fdSet))
    {
        receiver->Read();
        if (receiver->BufferLength() > 0)
        {
            do
            {
                // qmsg_enc_result = QMsgNetDecod
            } while ((total_consumed < 8192) && (consumed > 0));

            fprintf(stderr, "UI: Read %d bytes from SecProc: ", receiver->BufferLength());
            fwrite(receiver->Data(), 1, receiver->BufferLength(), stderr);
            fprintf(stderr, "\n");
        }
    }
}

void UserInterface::PrintMessage(const char *msg)
{
    printf(msg);
}

void UserInterface::PrintTimestampedMessage(const char *msg)
{
    tm *current_time = GetCurrentSystemTime();
    printf("%d:%d -!- %s", current_time->tm_hour, current_time->tm_min, msg);
}