#pragma once

#include <unistd.h>

#include "qmsg/encoder.h"

class SecApi {
private:
  QMsgEncoderContext *context;
  int sec2netFD;
  int net2secFD;

  void send(const QMsgNetMessage &message);

public:
  SecApi();
  ~SecApi();
  int getReadFD();

  void readMsg(QMsgNetMessage *message);
  void recvAsciiMsg(int team, int dev, int ch, uint8_t *msg, int msgLen);
};
