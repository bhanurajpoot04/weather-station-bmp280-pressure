// Minimal fake Arduino core for running the sketch on a PC. Not for real boards.
#pragma once
#include <cstdint>
#include <cstdio>
#include <string>

extern unsigned long fake_ms;
inline unsigned long millis() { return fake_ms; }

struct FakeSerial {
  std::string out;
  void begin(long) {}
  void print(const char* s) { out += s; }
  void print(float v, int digits = 2) { char b[32]; std::snprintf(b, sizeof b, "%.*f", digits, v); out += b; }
  void println(const char* s = "") { out += s; out += "\n"; }
  void println(float v, int digits = 2) { print(v, digits); out += "\n"; }
};
extern FakeSerial Serial;
