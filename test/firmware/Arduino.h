#pragma once

#include <cstdint>
#include <cstdio>
#include <string>

inline constexpr int INPUT = 0;
inline constexpr int OUTPUT = 1;
inline constexpr int INPUT_PULLUP = 2;
inline constexpr int LOW = 0;
inline constexpr int HIGH = 1;

inline uint32_t testMillis = 1;
inline uint16_t testProbe = 0;
inline int testPins[64]{};
// Optional physical keyboard model: reads depend on the actual scan outputs.
inline int (*testDigitalReadHook)(int) = nullptr;
inline uint32_t millis() { return testMillis; }
inline void pinMode(int, int) {}
inline void digitalWrite(int pin, int value) { testPins[pin] = value; }
inline int digitalRead(int pin) { return testDigitalReadHook ? testDigitalReadHook(pin) : testPins[pin]; }
inline uint16_t analogRead(int) { return testProbe; }

struct TestSerial
{
    template <class... T> void printf(const char *, T...) {}
    template <class T> void print(T) {}
    template <class T> void println(T) {}
};
inline TestSerial Serial;
