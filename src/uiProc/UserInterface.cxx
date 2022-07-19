#include "UserInterface.hh"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>

UserInterface::UserInterface(const int keyboard_fd,
                             const int sec_to_ui_fd,
                             const int ui_to_sec_id,
                             const unsigned int buffer_size) :
    selected_fd(0),
    is_running(false),
    update_draw(false)
{
    keyboard = new FdReader(keyboard_fd, buffer_size);
    receiver = new FdReader(sec_to_ui_fd, buffer_size);
    sender = new Sender(ui_to_sec_id);
    parser = new Parser();

    // HACK the defaults should be stored on the device
    // in the EEPROM and should be loaded at startup
    profile = new Profile("1234", "brett");

    if (QMsgEncoderInit(&sec_context))
    {
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
        Draw();
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
    AppendToUIMatrix(TimeStampMessage(welcome_str));
    DisplayHelpMessage();
    AppendToUIMatrix(TimeStampMessage(request_pin_str));
    AppendToUIMatrix("> ");

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
        AppendToUIMatrix(TimeStampMessage(help_desc[index]));
    }
}

void UserInterface::Draw()
{
    if (update_draw)
    {
        // HACK This is not very portable.. and may be different for the embedded
        // devices. However, at the moment it doesn't matter
        system("clear");
        for (int i = 0; i < draw_matrix.size(); i++)
        {
            fprintf(stderr, draw_matrix[i].c_str());
        }

        update_draw = false;
    }
}

void UserInterface::GetUserPin(std::string pin)
{
    if (profile->ComparePin(pin))
    {
        BuildMessageUI();
    }
    else
    {
        AppendToUIMatrix(pin + "\n");
        AppendToUIMatrix(TimeStampMessage(incorrect_pin_str));
        AppendToUIMatrix("> ");
    }
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
    // TODO consider moving the data from the keyboard into a buffer until handled or into a buffer queue so that it can get handled as needed?
    // Read messages from the keyboard, parse, and send it.
    if (keyboard->HasMessage(selected_fd, fdSet))
    {
        keyboard->Read();
        if (keyboard->BufferLength() > 0)
        {
            std::string data = keyboard->Data();
            // If we haven't received the user's pin yet, then wait here.
            if (!profile->PinAccepted())
            {
                GetUserPin(data);
                return;
            }

            // Parse the input for commands
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
                            fprintf(stderr, "Username has been set to %s\n",
                                argument_token);
                            profile->SetUsername(argument_token);
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

                return;
            }
            else
            {
                if (false)
                {
                    // TODO if we are not in a chat room then
                    // normal messages do nothing.
                    // and note we are not in a channel
                    return;
                }

                // TODO put timestamp onto the message.

                // Put message into the queue
                std::string msg = keyboard->Data();

                // Push the message into the messages vector
                std::vector<std::string> sub_messages = BuildMessage(profile->GetUsername(), msg);

                for (int i = 0; i < sub_messages.size(); i++)
                {
                    messages.push_back(sub_messages[i]);
                }

                BuildMessageUI();

                // Send to secure process
                sender->SendPlainMessage(keyboard->Data(), keyboard->BufferLength());
            }
        }
    }
}

void UserInterface::HandleReceiver(int selected_fd, fd_set fdSet)
{
    if (!profile->PinAccepted()) return;

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

            // TODO parse the input and apply it to the UI
        }
    }
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

void UserInterface::PrintMessage(const char *msg)
{
    fprintf(stderr, msg);
}

void UserInterface::PrintTimestampedMessage(const char *msg)
{
    tm *current_time = GetCurrentSystemTime();
    printf("%d:%d -!- %s", current_time->tm_hour, current_time->tm_min, msg);
}

void UserInterface::TimeStampMessage(std::string &msg)
{
    tm *current_time = GetCurrentSystemTime();
    msg = std::to_string(current_time->tm_hour) + ":" +
          std::to_string(current_time->tm_min) + " -!- " +
          msg;
}

std::string UserInterface::TimeStampMessage(const std::string msg)
{
    tm *current_time = GetCurrentSystemTime();
    return std::to_string(current_time->tm_hour) + ":" +
           std::to_string(current_time->tm_min) + " -!- " +
           msg;
}

void UserInterface::BuildMessageUI()
{
    // HACK this is only the concept currently
    // TODO calculate the number of lines high the console is?
    int32_t lines = 40;

    std::string team = "CTO Team";
    std::string current_channel = "Watercooler";

    std::vector<std::string> channels = {
        "Watercooler",
        "Watercooler",
        "General",
        "Hype project"
    };

    std::vector<std::string> users = {
        "brett",
        "tomas"
    };

    std::string append_str = "";

    std::vector<std::string> message_matrix;
    message_matrix.push_back("Welcome to Cisco Secure Messaging\n");

    uint32_t total_spaces = num_channel_spaces + num_messages_spaces + num_user_spaces;
    for (uint16_t i = 0; i < total_spaces; i++ )
    {
        append_str += "-";
    }
    append_str += "\n";
    message_matrix.push_back(append_str);

    // TODO calculate the spaces for this too.
    message_matrix.push_back(team + "                | #" + current_channel + "                                                                                      | Users         \n");

    int num_removed_spaces;
    for (int i = 0; i < lines; i++)
    {
        append_str = "";
        num_removed_spaces = 0;
        if (channels.size() > i)
        {
            if (channels[i].length() > num_channel_spaces)
                return; // TODO this is a problem

            // There are channels to print
            append_str += "#" + channels[i];

            // Calculate how many spaces we need instead before the separator
            num_removed_spaces = channels[i].length() + 1;
        }

        for (int j = 0; j < num_channel_spaces - num_removed_spaces; j++)
        {
            // Starting at j = 1 to account for the #
            append_str += " ";
        }

        // Append a bar after the channels
        append_str += "|";

        num_removed_spaces = 0;
        if (messages.size() > i)
        {
            // Messages to print these should be pre-parsed into individual lines prior to this
            // point because the splitting of the messages will completely mess with the
            // calculation on how to display the users section.
            append_str += " " + messages[i];

            // Get how long the message is.
            num_removed_spaces = messages[i].length() + 1;
        }

        // Append spaces to the message.
        for (int j = 1; j < num_messages_spaces - num_removed_spaces; j++)
        {
            append_str += " ";
        }

        append_str += "|";

        num_removed_spaces = 0;
        if (users.size() > i)
        {
            append_str += " @" + users[i];

            num_removed_spaces = users[i].length();
        }

        for (int j = 1; j < num_user_spaces - num_removed_spaces; j++)
        {
            append_str += " ";
        }

        append_str += "\n";

        message_matrix.push_back(append_str);
    }

    for (uint16_t i = 0; i < total_spaces; i++ )
    {
        append_str += "-";
    }
    append_str += "\n";
    message_matrix.push_back(append_str);
    message_matrix.push_back("> ");

    SetUIMatrix(message_matrix);
}

void UserInterface::SetUIMatrix(std::vector<std::string> matrix)
{
    draw_matrix = matrix;
    update_draw = true;
}

void UserInterface::SetUIMatrix(std::string msg)
{
    draw_matrix.clear();
    draw_matrix.push_back(msg);
    update_draw = true;
}

void UserInterface::AppendToUIMatrix(std::vector<std::string> matrix)
{
    for (int i = 0; i < matrix.size(); i++)
    {
        draw_matrix.push_back(matrix[i]);
    }
    update_draw = true;
}

void UserInterface::AppendToUIMatrix(std::string msg)
{
    draw_matrix.push_back(msg);
    update_draw = true;
}

std::vector<std::string> UserInterface::BuildMessage(std::string user_name, std::string message)
{
    // TODO add timestamps

    std::vector<std::string> sub_messages;
    std::string sub_msg = "";
    std::string split_msg = "<" + user_name + "> " + message;
    std::uint16_t num_sub_msgs = 0;

    // this if may not be needed
    if (split_msg.length() > num_messages_spaces)
    {
        std::cout << "here1" << std::endl;
        // This will not put the final bit of string onto the vector, that will
        // be handled at the end
        while (split_msg.length() > num_messages_spaces)
        {
            std::cout << "here2" << std::endl;
            bool delimiter_found = false;
            // Scan starting from the limit for the next delimiter
            // HACK minus 2 for cases where the start would be a space
            // and the word was already overflowing the spacing
            for (int i = num_messages_spaces - 2; i >= 0; i--)
            {
                // TODO add more delimiters
                if (split_msg[i] == ' ')
                {

                    std::cout << i << std::endl;
                    // If we have multiple sub messages append
                    // some spaces to the front
                    // if (num_sub_msgs > 1)
                    //     sub_msg += "  ";

                    sub_msg = split_msg.substr(0, i);

                    // Push onto the vector
                    sub_messages.push_back(sub_msg);

                    // Shift the window over
                    split_msg = "  " + split_msg.substr(i, split_msg.length() - i);
                    delimiter_found = true;
                    break;
                }
            }
        }
    }

    sub_messages.push_back(split_msg);

    for (int i = 0; i < sub_messages.size(); i++)
    {
        std::cout << sub_messages[i] << std::endl;
    }

    return sub_messages;
}