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

    // HACK this pin will be saved in the EEPROM so we may never actually do this
    // as it may be handled way before this point
    profile = new Profile("1234");

    if (QMsgEncoderInit(&sec_context))
    {
        // TODO Log and error out.
        fprintf(stderr, "Error - Failed to initialize encoder");
    }
}

UserInterface::~UserInterface()
{
    delete keyboard;
    delete receiver;
    delete sender;
    delete parser;
    delete profile;
    delete sec_context;
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

void UserInterface::Start()
{
    PrintMessage("Welcome to Cisco Secure Messaging\n\n");
    DisplayHelpMessage();
    PrintMessage("Please enter your pin\n");
    is_running = true;
}

void UserInterface::Stop()
{
    PrintMessage(bye_str.c_str());
    is_running = false;
}

void UserInterface::DisplayHelpMessage()
{
    for (int index = 0; index < sizeof(help_desc)/sizeof(char*); ++index){
        PrintTimestampedMessage(help_desc[index]);
    }
}

void UserInterface::GetUserPin()
{
    // PrintMessage("Please enter your pin");

    // std::string in_pin;
    // do
    // {
    //     if keyboard->HasMessage
    // } while (!profile->ComparePin());
}

bool UserInterface::IsValidChannel(std::string channel_id) {
    bool channel_exists = false;
    unsigned int idx = 0;
    for (unsigned int i = 0; i < all_channels.size(); i++)
    {
        if (channel_id == all_channels[i].Name())
        {
            channel_exists = true;
            idx = i;
            break;
        }
    }
    return channel_exists;
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
    // Read messages from the keyboard, parse, and send it.
    if (keyboard->HasMessage(selected_fd, fdSet))
    {
        keyboard->Read();
        if (keyboard->BufferLength() > 0)
        {
            // TODO remove
            fprintf(stderr, "UI: Read %d bytes from keyboard: ", keyboard->BufferLength());
            fwrite(keyboard->Data(), 1, keyboard->BufferLength(), stderr);
            fprintf(stderr, "\n");

            // If we haven't received the user's pin yet, then wait here.
            if (!profile->PinAccepted())
            {
                if (profile->ComparePin(keyboard->Data()))
                {
                    fprintf(stderr, "Pin accepted\n");
                }
                else
                {
                    fprintf(stderr, "Pin incorrect\n");
                }

                return;
            }

            // Parse the input for a command such as help
            // bool res = parser->Parse(keyboard->Data(), keyboard->BufferLength(), command);

            // parse the input for commands
            if (keyboard->Data()[0] == '/')
            {

                const char* command_token = strtok((keyboard->Data()), " ");
                fprintf(stderr, "UI: command received - %s\n", command_token);

                if (strcmp(command_token, commands[Command::help]) == 0)
                {
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
                            // TODO actually set it.
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
                    char* team_id_str = strtok(NULL, " ");
                    if (team_id_str == NULL) {
                        printf("Team id is missing, Try again");
                    }

                    printf("Connect to team %s", team_id_str);
                    if (team_id_str == NULL) {
                        printf("Error - team id is null, try again");
                        return;
                    }

                    unsigned int team_id = static_cast<unsigned int>(*team_id_str);
                    char* channel_id_str = strtok(NULL, " ");
                    printf("Connect to channel %s", channel_id_str);
                    if (channel_id_str == NULL) {
                            printf("Error - Channel id is missing, Try again");
                            return;
                    }
                    // if (!isValidChannel(channel_id_str)) {
                    //     printf("Enter a valid channel");
                    //     return;
                    // }
                    unsigned int channel_id = static_cast<unsigned int>(*channel_id_str);
                    sender->SendWatchMessage(team_id, channel_id);
                }
                else if (strcmp(command_token, commands[Command::join]) == 0)
                {
                    // Join a channel
                    std::string join_token(strtok(NULL, " "));
                    //fprintf(stderr, "Connecting to a room %s", join_token);

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
                            // TODO clean this up..
                            char channel_name[all_channels[idx].Name().length()];
                            strcpy(channel_name, all_channels[idx].Name().c_str());
                            sender->SendPlainMessage(channel_name, join_token.length());

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
                else if (strcmp(command_token, commands[Command::bye]) == 0)
                {
                    Stop();
                }

                else
                {
                    printf("Unknown command. The available commands are: \n");
                    DisplayHelpMessage();
                }

                // TODO
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
                sender->SendPlainMessage(keyboard->Data(), keyboard->BufferLength());
            }
        }
    }
}

void UserInterface::HandleReceiver(int selected_fd, fd_set fdSet)
{
    if (receiver->HasMessage(selected_fd, fdSet))
    {
        // receiver->Read(sec_fragment_size);
        if (receiver->BufferLength() > 0)
        {
            sec_fragment_size = 0;
            sec_total_consumed = 0;

            do
            {
                qmsg_enc_sec_res = QMsgNetDecodeMessage(sec_context,
                                                        (uint8_t *)receiver->Data() + sec_total_consumed,
                    receiver->BufferLength() - sec_total_consumed,
                    &sec_message,
                    &sec_consumed);

                sec_total_consumed += sec_consumed;

                if (qmsg_enc_sec_res == QMsgEncoderSuccess)
                {
                    // TODO
                    // We got a message yay.
                    fprintf(stderr, "encoder got %s", sec_message.u.receive_ascii_message.message.data);
                }

                if ((qmsg_enc_sec_res == QMsgEncoderInvalidMessage) ||
                    (qmsg_enc_sec_res == QMsgEncoderCorruptMessage))
                {
                    // TODO error
                    fprintf(stderr, "got an error");
                }

            } while ((sec_total_consumed < receiver->BufferLength())
                      && (sec_consumed > 0));

            if (sec_total_consumed < receiver->BufferLength())
            {
                receiver->SlideBuffer(sec_total_consumed);
                sec_fragment_size = receiver->BufferLength() - sec_total_consumed;
            }

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