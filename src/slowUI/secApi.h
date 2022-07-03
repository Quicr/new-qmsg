#pragma once

#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "qmsg/encoder.h"


class SecApi {
private:
  QMsgEncoderContext *context;
  int sec2uiFD;
  int ui2secFD;

  void send(const QMsgUIMessage &message);

public:
  SecApi();
  ~SecApi();

  int getReadFD();

  void watch(int team, int ch);
  void sendAsciiMsg(int team, int ch, uint8_t *msg, int msgLen);
  void readMsg(QMsgUIMessage *message);
};
