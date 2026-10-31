// Minimal fake Arduino core for running the class program on a PC. Not for real boards.
#pragma once
#include <cstdint>
#include <cstdio>
#include <string>

#define HIGH 1
#define LOW 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define FALLING 2

extern unsigned long fake_ms;
inline unsigned long millis() { return fake_ms; }
inline void delay(unsigned long) {}
inline void noInterrupts() {}
inline void interrupts() {}
inline void pinMode(uint8_t, uint8_t) {}
inline int digitalPinToInterrupt(int pin) { return pin; }

extern int pin_state[20];
inline void digitalWrite(uint8_t pin, uint8_t value) { pin_state[pin] = value; }

extern void (*isr_function)();
inline void attachInterrupt(int, void (*fn)(), int) { isr_function = fn; }

// Arduino String, reduced to what the program uses
struct String {
  std::string s;
  String(const char* c = "") : s(c) {}
};

struct FakeSerial {
  std::string out;
  void begin(long) {}
  void println(const char* t = "") { out += t; out += "\n"; }
};
extern FakeSerial Serial;
