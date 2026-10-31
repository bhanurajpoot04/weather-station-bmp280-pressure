# Components for this block

| Component | Qty | Role |
|---|---|---|
| Arduino Uno or Nano (ATmega328P) | 1 | Controller |
| BMP280 module (I2C, address 0x76 or 0x77) | 1 | Barometric pressure sensor |
| Breadboard and jumper wires | - | Wiring (used for this block, no PCB made) |
| USB cable | 1 | Power and Serial Monitor |
| 4.7 kohm resistors | 2 | I2C pull-ups, only if the module has none |
| 0.1 uF capacitor | 1 | Supply decoupling, if you move to a PCB |

The full class weather station also uses an SHT21, an anemometer and an RS485 link. See [`docs/system-overview.md`](../docs/system-overview.md).
