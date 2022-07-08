#include "Sender.hh"
#include <stdint.h>
#include <cstring>

Sender::Sender(int ui_to_sec_fd) : ui_to_sec_fd(ui_to_sec_fd)
{
    if (QMsgEncoderInit(&context)) throw "Failed to create sender context";
}

Sender::~Sender()
{
    QMsgEncoderDeinit(context);
}

void Sender::SendPlainMessage(char *buffer, const unsigned long buffer_length)
{
    // Encode the message
    QMsgUIMessage msg;
    msg.type = QMsgUISendASCIIMessage;
    msg.u.send_ascii_message.team_id = 0x01020304;
    msg.u.send_ascii_message.channel_id = 0x05060708;
    msg.u.send_ascii_message.message.length = buffer_length;
    msg.u.send_ascii_message.message.data = reinterpret_cast<uint8_t *>(buffer);

    SendEncoded(msg);
}

void Sender::SendWatchMessage(unsigned int team_id, unsigned int channel_id)
{
    QMsgUIMessage msg;
    msg.type = QMsgUIWatchChannel;
    msg.u.watch_channel.team_id = team_id;
    msg.u.watch_channel.channel_id = channel_id;

    SendEncoded(msg);
}

void Sender::SendEncoded(QMsgUIMessage msg)
{
    uint8_t send_buffer[1024]; // FIX this probably shouldn't be hard coded?
    std::size_t encoded_length;
    QMsgEncoderResult res = QMsgUIEncodeMessage(context,
                                                &msg,
                                                send_buffer,
                                                sizeof(send_buffer),
                                                &encoded_length);

    if (res != QMsgEncoderSuccess)
    {
        fprintf(stderr, "Error - Failed to send a ascii message");
        return;
    }

    // TODO remove, just debugging stuff
    fprintf(stderr, "Sending: %d bytes \n", encoded_length);
    for (int i = 0; i < encoded_length; i++)
    {
        fprintf(stderr, "%02X ", i, send_buffer[i]);
    }

    // Write the buffer to the fd
    // write(ui_to_sec_fd, send_buffer, encoded_length);
    fprintf(stderr, "\n");
}