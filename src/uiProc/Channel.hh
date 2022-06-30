#pragma once

#include <string>
#include <vector>

class Channel
{
public:
    Channel();
    ~Channel();

    std::string Name();

private:
    // Each channel should keep track of it's own messages
    std::string name;
    std::vector<std::string> messages;
};