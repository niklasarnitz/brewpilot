#pragma once

#include <cstdint>
#include <cstdio>
#include <string>

inline constexpr int INPUT = 0;
inline constexpr int OUTPUT = 1;
inline constexpr int INPUT_PULLUP = 2;
inline constexpr int LOW = 0;
inline constexpr int HIGH = 1;
inline constexpr int FALLING = 3;
#define IRAM_ATTR

inline uint32_t testMillis = 1;
inline uint16_t testProbe = 0;
inline int testPins[64]{};
inline int testPinModes[64]{};
// Like Arduino-ESP32 3.x, digitalWrite() is ignored until pinMode() claims the pin.
inline bool testPinIsGpio[64]{};
struct TestInterrupt { void (*callback)() = nullptr; int mode = 0; unsigned registrations = 0; };
inline TestInterrupt testInterrupts[64]{};
// Optional physical keyboard model: reads depend on the actual scan outputs.
inline int (*testDigitalReadHook)(int) = nullptr;
inline void (*testGpioChangeHook)() = nullptr;
inline uint32_t millis() { return testMillis; }
inline void pinMode(int pin, int mode)
{
    testPinModes[pin] = mode;
    testPinIsGpio[pin] = true;
    if (testGpioChangeHook)
        testGpioChangeHook();
}
inline void digitalWrite(int pin, int value)
{
    if (!testPinIsGpio[pin])
        return;
    testPins[pin] = value;
    if (testGpioChangeHook)
        testGpioChangeHook();
}
inline int digitalRead(int pin) { return testDigitalReadHook ? testDigitalReadHook(pin) : testPins[pin]; }
// Only the probe on GPIO34, excited via GPIO17, sees the simulated level.
inline uint16_t analogRead(int pin)
{
    return pin == 34 && testPinModes[17] == OUTPUT && testPins[17] == HIGH ? testProbe : 0;
}
inline void delay(uint32_t ms) { testMillis += ms; }
inline bool testLoopWdtEnabled = false;
inline void enableLoopWDT() { testLoopWdtEnabled = true; }
inline int digitalPinToInterrupt(int pin) { return pin; }
inline void attachInterrupt(int pin, void (*callback)(), int mode)
{
    testInterrupts[pin] = {callback, mode, testInterrupts[pin].registrations + 1};
}

struct TestSerial
{
    void begin(unsigned long) {}
    template <class... T> void printf(const char *, T...) {}
    template <class T> void print(T) {}
    template <class T> void println(T) {}
};
inline TestSerial Serial;
