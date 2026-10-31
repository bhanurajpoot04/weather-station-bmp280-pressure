// Runs the class transmitter program on a PC with fake sensors and a fake clock.
// Checks the LIVE and FINAL packet format, the averaging and the RS485 direction pin.
//
// Build and run from this folder:
//   g++ -std=c++11 -I. sim_final.cpp -o sim_final
//   ./sim_final
//
// The fake SHT21 returns raw values whose two bytes are equal (0x6B6B and 0x7070), so the
// result does not depend on the byte-order issue described in docs/known-issues.md.
#include "Arduino.h"
#include "Wire.h"
#include "SoftwareSerial.h"
#include "Adafruit_BMP280.h"

unsigned long fake_ms = 0;
int pin_state[20] = {0};
void (*isr_function)() = nullptr;
FakeSerial Serial;
FakeWire Wire;
int sht21_raw_temp = 0x6B6B;   // 27499 -> 26.9 degC
int sht21_raw_hum = 0x7070;    // 28784 -> 49 %RH
float fake_pressure_pa = 100800.0f;
std::string rs485_out;
int tx_while_receiving = 0;

#include "../../class_station_final/class_station_final.ino"

static int count(const std::string& hay, const std::string& needle) {
  int n = 0;
  for (size_t pos = hay.find(needle); pos != std::string::npos; pos = hay.find(needle, pos + 1)) n++;
  return n;
}

static int fail(const char* msg) {
  std::printf("FAIL: %s\n--- RS485 output ---\n%s\n", msg, rs485_out.c_str());
  return 1;
}

int main() {
  setup();
  if (isr_function == nullptr) return fail("anemometer interrupt was not attached");

  loop();   // first call sends the LIVE packet (no pulses yet)
  if (rs485_out != "LIVE,26.9,49,1008,0.0,Calm\n") return fail("LIVE packet");

  // 10 minutes, with 30 anemometer pulses in every 10-second window
  for (fake_ms = 1000; fake_ms <= 600000UL; fake_ms += 1000) {
    if (fake_ms % 10000 == 0) {
      for (int i = 0; i < 30; i++) isr_function();
    }
    loop();
  }

  if (count(rs485_out, "LIVE,") != 1) return fail("LIVE must be sent once");
  if (count(rs485_out, "FINAL,") != 1) return fail("exactly one FINAL packet in 10 minutes");
  if (rs485_out.find("FINAL,26.9,49,1008,1.1,LightAir\n") == std::string::npos) return fail("FINAL packet");
  if (tx_while_receiving != 0) return fail("data was sent while the RS485 driver was disabled");

  std::printf("sim_final: passed\n  %s", rs485_out.c_str());
  return 0;
}
