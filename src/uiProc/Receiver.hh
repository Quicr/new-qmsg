#pragma once

#include "FdReader.hh"

class Receiver
{
public:
    Receiver(int fd);
    ~Receiver();

private:
    FdReader* sec_reader;

};
