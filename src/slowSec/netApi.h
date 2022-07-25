#pragma once

#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <vector>

#include "qmsg/encoder.h"

class NetApi {
private:
  QMsgEncoderContext *context;
  int sec2netFD;
  int net2secFD;

  void send(const QMsgNetMessage &message);

public:
  NetApi();
  ~NetApi();

  int getReadFD();

  void watch(int team, std::vector<QMsgDeviceID> &deviceList, int ch);
  void sendAsciiMsg(int team, int device, int ch, uint8_t *msg, int msgLen);
  void readMsg(QMsgNetMessage *message);
};
