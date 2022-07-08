#pragma once

#include "Channel.hh"

class Room
{
public:
    Room();
    ~Room();

    void SetChannels();
    void AddChannel();
    void RemoveChannel();
    void RemoveChannels();

private:
    Channel* channels;
};