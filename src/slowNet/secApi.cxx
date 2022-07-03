
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

#include "secApi.h"

void SecApi::send(const QMsgNetMessage &message) {
  uint8_t encodeBuffer[1024];
  size_t encodeLen;
  QMsgEncoderResult err;
  err = QMsgNetEncodeMessage(context, &message, encodeBuffer,
                             sizeof(encodeBuffer), &encodeLen);
  assert(err == QMsgEncoderSuccess);

  uint32_t sendLen = encodeLen;
  ssize_t n = write(net2secFD, &sendLen, sizeof(sendLen));
  assert(n == sizeof(sendLen));
  n = write(net2secFD, encodeBuffer, sendLen);
  assert(n == sendLen);
}

int SecApi::getReadFD() { return sec2netFD; }

SecApi::SecApi() {
  QMsgEncoderInit(&context);

  sec2netFD = open("/tmp/pipe-s2n", O_RDONLY, O_NONBLOCK);
  assert(sec2netFD >= 0);
  std::clog << "SEC: Got pipe from uiProc" << std::endl;

  net2secFD = open("/tmp/pipe-n2s", O_WRONLY, O_NONBLOCK);
  assert(net2secFD >= 0);
  std::clog << "SEC: Got pipe to uiProc" << std::endl;
}

SecApi::~SecApi() {
  assert(context);
  QMsgEncoderDeinit(context);
  context = nullptr;
};

void SecApi::readMsg(QMsgNetMessage *message) {
  assert(message);

  const int bufSize = 128;
  uint8_t uiBuf[bufSize];

  uint32_t msgLen = 0;
  ssize_t num = read(sec2netFD, &msgLen, sizeof(msgLen));
  if ( num == 0 ) {
    message->type = QMsgNetInvalid;
    return;
  }
  assert(num == sizeof(msgLen));
  assert(msgLen <= bufSize);
  num = read(sec2netFD, uiBuf, msgLen);
  assert(num == msgLen);

  QMsgEncoderResult err;
  size_t consumed;

  err = QMsgNetDecodeMessage(context, uiBuf, msgLen, message, &consumed);
  assert(err == QMsgEncoderSuccess);
  assert(consumed == msgLen);
}

void SecApi::recvAsciiMsg(int team, int dev, int ch, uint8_t *msg, int msgLen) {
  assert(msg);
  assert(msgLen > 0);

  QMsgNetMessage message{};
  message.type = QMsgNetReceiveASCIIMessage;
  message.u.receive_ascii_message.team_id = team;
  message.u.receive_ascii_message.device_id = dev;
  message.u.receive_ascii_message.channel_id = ch;
  message.u.receive_ascii_message.message.length = msgLen;
  message.u.receive_ascii_message.message.data = msg;

  send(message);
}
