
#include <assert.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "qmsg/encoder.h"

#include "uiApi.h"


void UiApi::send(const QMsgUIMessage &message) {
  uint8_t encodeBuffer[1024];
  size_t encodeLen;
  QMsgEncoderResult err;
  err = QMsgUIEncodeMessage(context, &message, encodeBuffer,
                            sizeof(encodeBuffer), &encodeLen);
  assert(err == QMsgEncoderSuccess);

  uint32_t sendLen = encodeLen;
  ssize_t n = write(sec2uiFD, &sendLen, sizeof(sendLen));
  assert(n == sizeof(sendLen));
  n = write(sec2uiFD, encodeBuffer, sendLen);
  assert(n == sendLen);
}

int UiApi::getReadFD() { return ui2secFD; }

UiApi::UiApi() {
  QMsgEncoderInit(&context);

  sec2uiFD = open("/tmp/pipe-s2u", O_WRONLY, O_NONBLOCK);
  assert(sec2uiFD >= 0);
  std::clog << "SEC: Got pipe from uiProc" << std::endl;

  ui2secFD = open("/tmp/pipe-u2s", O_RDONLY, O_NONBLOCK);
  assert(ui2secFD >= 0);
  std::clog << "SEC: Got pipe to uiProc" << std::endl;
}

UiApi::~UiApi() {
  assert(context);
  QMsgEncoderDeinit(context);
  context = nullptr;
};

void UiApi::readMsg(QMsgUIMessage *message) {
  assert(message);

  const int bufSize = 128;
  uint8_t uiBuf[bufSize];

  uint32_t msgLen = 0;
  ssize_t num = read(ui2secFD, &msgLen, sizeof(msgLen));
   if ( num == 0 ) {
    message->type = QMsgUIInvalid;
    return;
  }
  assert(num == sizeof(msgLen));
  assert(msgLen <= bufSize);
  num = read(ui2secFD, uiBuf, msgLen);
  assert(num == msgLen);

  QMsgEncoderResult err;
  size_t consumed;

  err = QMsgUIDecodeMessage(context, uiBuf, msgLen, message, &consumed);
  assert(err == QMsgEncoderSuccess);
  assert(consumed == msgLen);
}

void  UiApi::recvAsciiMsg(int team, int dev, int ch, uint8_t *msg, int msgLen) {
    assert(msg);
  assert(msgLen > 0);

  QMsgUIMessage message{};
  message.type = QMsgUIReceiveASCIIMessage;
  message.u.receive_ascii_message.team_id = team;
  message.u.receive_ascii_message.device_id = dev;
  message.u.receive_ascii_message.channel_id = ch;
  message.u.receive_ascii_message.message.length = msgLen;
  message.u.receive_ascii_message.message.data = msg;

  send(message);
}
