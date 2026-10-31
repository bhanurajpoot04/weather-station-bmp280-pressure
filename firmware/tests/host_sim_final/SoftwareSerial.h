// Fake RS485 serial port that records what is sent and whether the driver was enabled.
#pragma once
#include <cstdio>
#include <string>
#include "Arduino.h"

extern std::string rs485_out;
extern int tx_while_receiving;   // bytes sent while the RTS pin (3) was LOW

struct SoftwareSerial {
  SoftwareSerial(int, int) {}
  void begin(long) {}
  void note() { if (pin_state[3] != HIGH) tx_while_receiving++; }
  void print(const char* t) { note(); rs485_out += t; }
  void print(float v, int digits = 2) { note(); char b[32]; std::snprintf(b, sizeof b, "%.*f", digits, v); rs485_out += b; }
  void println(const String& t) { note(); rs485_out += t.s; rs485_out += "\n"; }
};
