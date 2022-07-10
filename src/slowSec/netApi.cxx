
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

#include "netApi.h"

void NetApi::send(const QMsgNetMessage &message) {
  uint8_t encodeBuffer[1024];
  size_t encodeLen;
  QMsgEncoderResult err;
  err = QMsgNetEncodeMessage(context, &message, encodeBuffer,
                             sizeof(encodeBuffer), &encodeLen);
  assert(err == QMsgEncoderSuccess);

  QMsgLength sendLen = encodeLen;
  ssize_t n = write(sec2netFD, &sendLen, sizeof(sendLen));
  assert(n == sizeof(sendLen));
  n = write(sec2netFD, encodeBuffer, sendLen);
  assert(n == sendLen);
}

int NetApi::getReadFD() { return net2secFD; }

NetApi::NetApi() {
  QMsgEncoderInit(&context);

  sec2netFD = open("/tmp/pipe-s2n", O_WRONLY, O_NONBLOCK);
  assert(sec2netFD >= 0);
  std::clog << "SEC: Got pipe to netProc" << std::endl;

  net2secFD = open("/tmp/pipe-n2s", O_RDONLY, O_NONBLOCK);
  assert(net2secFD >= 0);
  std::clog << "SEC: Got pipe from netProc" << std::endl;
}

NetApi::~NetApi() {
  assert(context);
  QMsgEncoderDeinit(context);
  context = nullptr;
};

void NetApi::watch(int team, std::vector<QMsgDeviceID> &deviceList, int ch) {
  QMsgNetMessage message{};

  assert(sizeof(*(message.u.watch_devices.device_list.device_list)) ==
         sizeof(QMsgDeviceID));

  message.type = QMsgNetWatchDevices;
  message.u.watch_devices.team_id = team;
  message.u.watch_devices.channel_id = ch;
  message.u.watch_devices.device_list.num_devices = deviceList.size();
  message.u.watch_devices.device_list.device_list = deviceList.data();

  send(message);
}

void NetApi::sendAsciiMsg(int team, int device, int ch, uint8_t *msg,
                          int msgLen) {
  assert(msg);
  assert(msgLen > 0);

  QMsgNetMessage message{};
  message.type = QMsgNetSendASCIIMessage;
  message.u.send_ascii_message.team_id = team;
  message.u.send_ascii_message.device_id = device;
  message.u.send_ascii_message.channel_id = ch;
  message.u.send_ascii_message.message.length = msgLen;
  message.u.send_ascii_message.message.data = msg;

  send(message);
}

void NetApi::readMsg(QMsgNetMessage *message) {
  assert(message);

  const int bufSize = 128;
  uint8_t uiBuf[bufSize];

  QMsgLength msgLen = 0;
  ssize_t num = read(net2secFD, &msgLen, sizeof(msgLen));
  if ( num == 0 ) {
    message->type = QMsgNetInvalid;
    return;
  }
  assert(num == sizeof(msgLen));
  if ( msgLen >= bufSize ) { std::cerr << "msgLen=" << msgLen << std::endl; }
  assert(msgLen <= bufSize);
  num = read(net2secFD, uiBuf, msgLen);
  assert(num == msgLen);

  QMsgEncoderResult err;
  size_t consumed;

  err = QMsgNetDecodeMessage(context, uiBuf, msgLen, message, &consumed);
  assert(err == QMsgEncoderSuccess);
  assert(consumed == msgLen);
}
