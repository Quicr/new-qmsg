#pragma once

#include "Channel.hh"

class Team
{
public:
    Team();
    ~Team();

private:
    Channel* channels_arr;
};