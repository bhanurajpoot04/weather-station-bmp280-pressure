// Fake I2C bus that answers like an SHT21 with fixed raw values.
#pragma once
#include <cstdint>

extern int sht21_raw_temp;   // raw 16-bit temperature value to return
extern int sht21_raw_hum;    // raw 16-bit humidity value to return

struct FakeWire {
  int requests = 0;
  uint8_t buf[2] = {0, 0};
  int left = 0;
  void begin() {}
  void beginTransmission(int) {}
  void write(int) {}
  int endTransmission() { return 0; }
  void requestFrom(int, int) {
    int raw = (requests++ % 2 == 0) ? sht21_raw_temp : sht21_raw_hum;
    buf[0] = (raw >> 8) & 0xFF;
    buf[1] = raw & 0xFF;
    left = 2;
  }
  int available() { return left; }
  int read() { return buf[2 - left--]; }
};
extern FakeWire Wire;
