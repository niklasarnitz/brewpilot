#pragma once

#include <cstdint>
#include <cstdio>
#include <string>

#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define LOW 0
#define HIGH 1

inline uint32_t testMillis = 1;
inline uint16_t testProbe = 0;
inline int testPins[64]{};
inline uint32_t millis() { return testMillis; }
inline void pinMode(int, int) {}
inline void digitalWrite(int pin, int value) { testPins[pin] = value; }
inline int digitalRead(int pin) { return testPins[pin]; }
inline uint16_t analogRead(int) { return testProbe; }

struct TestSerial
{
    template <class... T> void printf(const char *, T...) {}
    template <class T> void print(T) {}
    template <class T> void println(T) {}
};
inline TestSerial Serial;
