/*
  BMP280 barometric pressure block - weather station class project

  This is the sketch our group used for the pressure block.
  Comments are translated to English; the logic is unchanged.

  Every 10 s it reads pressure in hPa and keeps the samples in two stages:
    15 samples (2.5 min)  -> one average
    4 averages (10 min)   -> FINAL value
  It prints a LIVE value once at start-up and a FINAL value every 10 minutes,
  using the same "TYPE,value" style as the rest of the class weather station.

  Libraries (Arduino Library Manager): Adafruit BMP280 Library
  (it also installs Adafruit BusIO and Adafruit Unified Sensor).
  Serial Monitor: 9600 baud.
*/
#include <Wire.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp;

float pres_2_5[15];
int idx_2_5 = 0;
float pres_10[4];
int idx_10 = 0;
unsigned long lastRead = 0;

// ---- Keep this function when merging with the rest of the class code ----
float readPressure() {
  return bmp.readPressure() / 100.0;   // Pa -> hPa
}

void setup() {
  Serial.begin(9600);
  if (!bmp.begin(0x76)) {
    if (!bmp.begin(0x77)) {
      Serial.println("BMP FAIL");
      while (1);
    }
  }
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_NONE,    // temperature off
                  Adafruit_BMP280::SAMPLING_X16,     // pressure
                  Adafruit_BMP280::FILTER_X4,
                  Adafruit_BMP280::STANDBY_MS_500);
  Serial.println("BMP PRESSURE READY");
  Serial.print("LIVE,");
  Serial.println(readPressure(), 0);
}

void loop() {
  if (millis() - lastRead >= 10000) {
    lastRead = millis();
    float p = readPressure();
    pres_2_5[idx_2_5++] = p;
    Serial.print("Sample: "); Serial.println(p, 1);

    if (idx_2_5 == 15) {                       // 2.5 min
      float s = 0;
      for (int i = 0; i < 15; i++) s += pres_2_5[i];
      pres_10[idx_10++] = s / 15;
      idx_2_5 = 0;

      if (idx_10 == 4) {                       // 10 min
        float f = 0;
        for (int i = 0; i < 4; i++) f += pres_10[i];
        Serial.print("FINAL,");
        Serial.println(f / 4, 0);
        idx_10 = 0;
      }
    }
  }
}
