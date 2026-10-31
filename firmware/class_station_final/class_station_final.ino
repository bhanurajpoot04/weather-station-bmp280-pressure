/*
  Class weather station - transmitter program (final version)

  Written by the class, one block per group. Our group's block is the BMP280 pressure part
  (the bmp object, bmp.readPressure() and the pres_* arrays). Blocks from other groups are
  wind speed, the Beaufort scale, SHT21 temperature and humidity, and the RS485 link.

  This copy has formatting and copy/paste artifacts fixed so that it compiles.
  The logic is unchanged, and the known issues are listed in docs/known-issues.md.
  The receiver and LCD program is not included.
*/
#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_BMP280.h>

#define SHT21_ADDR 0x40
#define RX_PIN 4
#define TX_PIN 5
#define RTS_PIN 3

SoftwareSerial rs485(RX_PIN, TX_PIN);
Adafruit_BMP280 bmp;

// -------- WIND ---------
volatile int pulseCount = 0;
float radius = 0.056;

// -------- TIMING ---------
unsigned long lastRead = 0;

// -------- FLAGS ---------
bool firstReadingSent = false;

// -------- ARRAYS ---------
float temp_2_5[15], hum_2_5[15], pres_2_5[15], wind_2_5[15];
int idx_2_5 = 0;
float temp_10[4], hum_10[4], pres_10[4], wind_10[4];
int idx_10 = 0;

// -------- INTERRUPT ---------
void countRotation() {
  pulseCount++;
}

// -------- BEAUFORT ---------
String beaufort(float v) {
  if (v < 0.5) return "Calm";
  else if (v < 1.6) return "LightAir";
  else if (v < 3.4) return "LightBreeze";
  else if (v < 5.5) return "Gentle";
  else if (v < 8.0) return "Moderate";
  else if (v < 10.8) return "Fresh";
  else if (v < 13.9) return "Strong";
  else return "Storm";
}

// -------- SHT21 ---------
bool readSHT21(float &temperature, float &humidity) {
  Wire.beginTransmission(SHT21_ADDR);
  Wire.write(0xF3);
  if (Wire.endTransmission() != 0) return false;
  delay(100);
  Wire.requestFrom(SHT21_ADDR, 2);
  if (Wire.available() < 2) return false;
  uint16_t rawTemp = (Wire.read() << 8) | Wire.read();

  Wire.beginTransmission(SHT21_ADDR);
  Wire.write(0xF5);
  if (Wire.endTransmission() != 0) return false;
  delay(100);
  Wire.requestFrom(SHT21_ADDR, 2);
  if (Wire.available() < 2) return false;
  uint16_t rawHum = (Wire.read() << 8) | Wire.read();

  temperature = -46.85 + 175.72 * rawTemp / 65536.0;
  humidity    = -6 + 125.0 * rawHum / 65536.0;
  return true;
}

void setup() {
  Serial.begin(9600);
  rs485.begin(9600);
  Wire.begin();
  pinMode(RTS_PIN, OUTPUT);
  digitalWrite(RTS_PIN, LOW);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), countRotation, FALLING);

  if (!bmp.begin(0x76)) {
    if (!bmp.begin(0x77)) {
      Serial.println("BMP FAIL");
      while (1);
    }
  }
  Serial.println("SYSTEM READY");
}

void loop() {
  // -------- FIRST LIVE DATA -------
  if (!firstReadingSent) {
    float temp = 0, hum = 0;
    readSHT21(temp, hum);
    float pressure = bmp.readPressure() / 100.0;
    noInterrupts();
    int pulses = pulseCount;
    pulseCount = 0;
    interrupts();
    float freq = pulses / 2.0;
    float wind = 2 * 3.1416 * radius * freq;
    String bft = beaufort(wind);

    digitalWrite(RTS_PIN, HIGH);
    rs485.print("LIVE,");
    rs485.print(temp, 1); rs485.print(",");
    rs485.print(hum, 0); rs485.print(",");
    rs485.print(pressure, 0); rs485.print(",");
    rs485.print(wind, 1); rs485.print(",");
    rs485.println(bft);
    delay(10);
    digitalWrite(RTS_PIN, LOW);
    firstReadingSent = true;
  }

  // -------- EVERY 10 SEC -------
  if (millis() - lastRead >= 10000) {
    lastRead = millis();
    float temp = 0, hum = 0;
    readSHT21(temp, hum);
    float pressure = bmp.readPressure() / 100.0;
    noInterrupts();
    int pulses = pulseCount;
    pulseCount = 0;
    interrupts();
    float freq = pulses / 10.0;
    float wind = 2 * 3.1416 * radius * freq;

    temp_2_5[idx_2_5] = temp;
    hum_2_5[idx_2_5]  = hum;
    pres_2_5[idx_2_5] = pressure;
    wind_2_5[idx_2_5] = wind;
    idx_2_5++;

    // -------- 2.5 MIN -------
    if (idx_2_5 == 15) {
      float sumT = 0, sumH = 0, sumP = 0, sumW = 0;
      for (int i = 0; i < 15; i++) {
        sumT += temp_2_5[i];
        sumH += hum_2_5[i];
        sumP += pres_2_5[i];
        sumW += wind_2_5[i];
      }
      float avgT = sumT / 15;
      float avgH = sumH / 15;
      float avgP = sumP / 15;
      float avgW = sumW / 15;

      temp_10[idx_10] = avgT;
      hum_10[idx_10]  = avgH;
      pres_10[idx_10] = avgP;
      wind_10[idx_10] = avgW;
      idx_10++;
      idx_2_5 = 0;

      // -------- 10 MIN FINAL -------
      if (idx_10 == 4) {
        float fT = 0, fH = 0, fP = 0, fW = 0;
        for (int i = 0; i < 4; i++) {
          fT += temp_10[i];
          fH += hum_10[i];
          fP += pres_10[i];
          fW += wind_10[i];
        }
        float finalT = fT / 4;
        float finalH = fH / 4;
        float finalP = fP / 4;
        float finalW = fW / 4;
        String bft = beaufort(finalW);

        digitalWrite(RTS_PIN, HIGH);
        rs485.print("FINAL,");
        rs485.print(finalT, 1); rs485.print(",");
        rs485.print(finalH, 0); rs485.print(",");
        rs485.print(finalP, 0); rs485.print(",");
        rs485.print(finalW, 1); rs485.print(",");
        rs485.println(bft);
        delay(10);
        digitalWrite(RTS_PIN, LOW);
        idx_10 = 0;
      }
    }
  }
}
