#pragma once
class Parser
{
public:
    Parser();
    ~Parser();

    bool Parse(char* buffer, const unsigned int buffer_length, int &command);

private:
};