#pragma once

#include <cstdint>
#include <map>
#include <string>

// The external NVS boundary only: application helpers remain real in tests.
inline std::map<std::string, uint32_t> testStorage;
inline bool testFailOpen = false;
inline bool testFailWrite = false;

class Preferences
{
    bool opened = false;

public:
    bool begin(const char *, bool)
    {
        opened = !testFailOpen;
        return opened;
    }
    void end() { opened = false; }
    uint32_t getULong(const char *key, uint32_t fallback)
    {
        return opened && testStorage.count(key) ? testStorage.at(key) : fallback;
    }
    size_t putULong(const char *key, uint32_t value)
    {
        if (!opened || testFailWrite)
            return 0;
        testStorage[key] = value;
        return sizeof(uint32_t);
    }
};
