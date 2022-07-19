#include "Profile.hh"

// This will be saved on the device's EEPROM
Profile::Profile(std::string pin) : pin(pin)
{

}
Profile::Profile(std::string pin, std::string username) :
    pin(pin), username(username)
{

}

Profile::~Profile()
{}


bool Profile::ComparePin(std::string pin)
{
    // Set up this way so that if at some point the pin is compared again
    // it doesn't clear the status.
    if (this->pin == pin)
    {
        pin_accepted = true;
        return true;
    }
    return false;
}

bool Profile::PinAccepted()
{
    return pin_accepted;
}

std::string Profile::GetUsername()
{
    return username;
}

void Profile::SetPin(std::string pin)
{
    // This will eventually be saved into the embedded device EERPROM
    this->pin = pin;
}

void Profile::SetUsername(std::string username)
{
    this->username = username;
}