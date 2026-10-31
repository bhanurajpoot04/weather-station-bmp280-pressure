#pragma once
extern float fake_pressure_pa;

struct Adafruit_BMP280 {
  bool begin(int) { return true; }
  float readPressure() { return fake_pressure_pa; }
};
