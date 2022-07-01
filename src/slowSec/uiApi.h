#pragma once

#include <unistd.h>

#include "qmsg/encoder.h"

class UiApi {
private:
  QMsgEncoderContext *context;
  int sec2uiFD;
  int ui2secFD;

  void send(const QMsgUIMessage &message);

public:
  UiApi();
  ~UiApi();
  int getReadFD();

  void readMsg(QMsgUIMessage *message);
  void recvAsciiMsg(int team, int dev, int ch, uint8_t *msg, int msgLen);
};
