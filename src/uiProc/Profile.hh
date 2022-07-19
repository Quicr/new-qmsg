#pragma once

#include <string>


class Profile
{
public:
    Profile(std::string pin);
    Profile(std::string pin, std::string username);
    ~Profile();

    bool ComparePin(std::string pin);
    std::string GetUsername();
    bool PinAccepted();


    void SetPin(std::string pin);
    void SetUsername(std::string username);

private:
    std::string pin;
    std::string username;

    bool pin_accepted;
};
