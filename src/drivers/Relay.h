#pragma once

class Relay
{
private:
    int pin;

    const char *name;

    bool isInverted;
    bool enabled = false;

public:
    Relay(int pin, const char *name, bool isInverted = false);

    void setEnabled(bool newState);

    bool isEnabled();
};
