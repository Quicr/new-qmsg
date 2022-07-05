#pragma once

#include <slower.h>

enum class NamePath : uint8_t {
  message = 1,
  keyPackage = 2,
  welcome = 3,
  commitAll = 4,
  commitOne = 5
};

class Name {
private:
  ShortName name;
  const uint32_t itad = 0x000088; // number from IANA TRIP ITAD registry TODO
  const uint8_t qmsgAppID = 0x88; // app ID for QMsg stuff in this ITAD

public:
  Name(ShortName &n);
  Name(NamePath path, uint32_t org, uint32_t team,
       uint64_t fingerprint); // key package & welcome
  //Name(NamePath path, uint32_t org, uint32_t team, uint32_t epoch,
  //     uint32_t rand); // commit-all
  //Name(NamePath path, uint32_t org, uint32_t team, uint32_t device,
  //     uint32_t epoch, uint32_t rand); // commit-one
  Name(NamePath path, uint32_t org, uint32_t team, uint16_t channel,
       uint32_t device, uint32_t msgNum = 0); // msg data

  uint32_t orginID();
  uint8_t appID();
  NamePath path();
  uint32_t org();
  uint32_t team();
  uint32_t channel();
  uint32_t device();
  uint32_t msgNum();
  uint64_t fingerprint();
  uint32_t epoch();
  uint32_t rand();

  ShortName &shortName();

  std::string shortString();

  std::string longString();
};
