// Fake BMP280 driver with the same names the sketch uses.
#pragma once
#include <vector>

extern float fake_pressure_pa;        // what the "sensor" reports, in pascals
extern int fake_working_address;      // I2C address that answers (0x76 or 0x77)
extern std::vector<int> begin_calls;  // addresses the sketch tried, in order

class Adafruit_BMP280 {
 public:
  enum sensor_mode { MODE_SLEEP = 0, MODE_FORCED = 1, MODE_NORMAL = 3 };
  enum sensor_sampling { SAMPLING_NONE = 0, SAMPLING_X1 = 1, SAMPLING_X16 = 5 };
  enum sensor_filter { FILTER_OFF = 0, FILTER_X4 = 2 };
  enum standby_duration { STANDBY_MS_500 = 4 };

  bool begin(int addr) { begin_calls.push_back(addr); return addr == fake_working_address; }
  void setSampling(sensor_mode, sensor_sampling, sensor_sampling, sensor_filter, standby_duration) {}
  float readPressure() { return fake_pressure_pa; }
};
