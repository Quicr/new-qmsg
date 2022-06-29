#pragma once

class Channel
{
public:
    Channel();
    ~Channel();

private:
    // Each channel should keep track of it's own messages
    char messages;
};