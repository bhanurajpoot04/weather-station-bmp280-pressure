// Runs the real bmp280_pressure.ino on a PC with a fake clock and a fake sensor.
//
// Build and run from this folder:
//   g++ -std=c++11 -I. sim_test.cpp -o sim_test
//   ./sim_test step      # pressure changes halfway, checks LIVE and FINAL values
//   ./sim_test addr77    # sensor only answers on 0x77, checks the address fallback
#include "Arduino.h"
#include "Adafruit_BMP280.h"

unsigned long fake_ms = 0;
FakeSerial Serial;
float fake_pressure_pa = 100000.0f;
int fake_working_address = 0x76;
std::vector<int> begin_calls;

#include "../../bmp280_pressure/bmp280_pressure.ino"

static int count(const std::string& hay, const std::string& needle) {
  int n = 0;
  for (size_t pos = hay.find(needle); pos != std::string::npos; pos = hay.find(needle, pos + 1)) n++;
  return n;
}

static int fail(const char* msg) { std::printf("FAIL: %s\n%s\n", msg, Serial.out.c_str()); return 1; }

int main(int argc, char** argv) {
  std::string mode = argc > 1 ? argv[1] : "step";

  if (mode == "addr77") {
    fake_working_address = 0x77;
    setup();
    if (begin_calls.size() != 2 || begin_calls[0] != 0x76 || begin_calls[1] != 0x77) return fail("address fallback");
    if (count(Serial.out, "BMP FAIL") != 0 || count(Serial.out, "BMP PRESSURE READY") != 1) return fail("start-up text");
    std::printf("addr77: passed\n");
    return 0;
  }

  // "step": 1000 hPa for the first 300 s, then 1010 hPa, run for 10 minutes
  setup();
  for (fake_ms = 0; fake_ms <= 600000UL; fake_ms += 1000) {
    fake_pressure_pa = (fake_ms <= 300000UL) ? 100000.0f : 101000.0f;
    loop();
  }
  if (count(Serial.out, "LIVE,1000\n") != 1) return fail("LIVE line");
  if (count(Serial.out, "Sample: ") != 60) return fail("expected 60 samples in 10 minutes");
  if (count(Serial.out, "FINAL,1005\n") != 1) return fail("FINAL line should be 1005");
  std::printf("step: passed (60 samples, LIVE,1000 once, FINAL,1005 once)\n");
  return 0;
}
