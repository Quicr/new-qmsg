class Parser
{
public:
    Parser();
    ~Parser();

    typedef enum
    {
        error = -1,
        help,
        text,
        join,
        direct,

    } Command;

    bool Parse(char* buffer, const unsigned int buffer_length, Command &command);

private:
};