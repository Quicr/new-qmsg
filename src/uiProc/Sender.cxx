#include "Sender.hh"
#include <iostream>
#include <cstring>

Sender::Sender(int ui_to_sec_fd) : ui_to_sec_fd(ui_to_sec_fd)
{
    if (QMsgEncoderInit(&context)) throw "Failed to create sender context";
}

Sender::~Sender()
{
    QMsgEncoderDeinit(context);
}

void Sender::SendMessage(char *buffer, const unsigned long buffer_length)
{
    // Encode the message
    QMsgUIMessage message{};
    message.type = QMsgUISendASCIIMessage;
    message.u.send_ascii_message.team_id = 0x01020304;
    message.u.send_ascii_message.channel_id = 0x05060708;
    message.u.send_ascii_message.message.length = buffer_length;
    message.u.send_ascii_message.message.data = reinterpret_cast<std::uint8_t *>(buffer);

    std::size_t encoded_length;

    QMsgEncoderResult res = QMsgUIEncodeMessage(context,
                                                &message,
                                                (uint8_t *)buffer,
                                                buffer_length,
                                                &encoded_length);

    // Write the buffer to the fd
    fprintf(stderr, "Sending: %d", buffer_length);
    write(ui_to_sec_fd, buffer, buffer_length);
    fprintf(stderr, "\n");
}

void Sender::EncodeMessage(char *buffer, const unsigned int buffer_length)
{
}
 void Sender::SendWatchMessage(unsigned int team_id, unsigned int channel_id) {
     QMsgUIMessage message{};
     message.type = QMsgUIWatchChannel;
     message.u.watch_channel.team_id = team_id;
     message.u.watch_channel.channel_id = channel_id;

     std::size_t encoded_length;
     uint8_t buffer[1024];
     QMsgEncoderResult res = QMsgUIEncodeMessage(context,
                                                 &message,
                                                 (uint8_t *)buffer,
                                                 1024,
                                                 &encoded_length);
     if(res != QMsgEncoderSuccess) {
         fprintf(stderr, "SendWatcHMessage: encode error %d", res);
         return;
     }
     fprintf(stderr, "Sending: %d",encoded_length);
     write(ui_to_sec_fd, buffer, encoded_length);
     fprintf(stderr, "\n");
 }