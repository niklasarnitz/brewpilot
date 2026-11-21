#pragma once

class Solenoid
{
private:
    int pin;

    const char *name;

    bool isInverted;
    bool open = false;

public:
    Solenoid(int pin, const char *name, bool isInverted = false);

    void setOpen(bool newState);

    bool isOpen();
};
