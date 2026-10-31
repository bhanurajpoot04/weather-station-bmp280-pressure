# Class weather station: system overview

The class built an Arduino weather station in blocks. Each group built and tested one block, and the blocks were then merged into one program. **That merged program is not part of this repository.** This page summarises how the blocks fit together so the pressure block makes sense in context.

## Blocks

| Block | Meaning |
|---|---|
| WS | Wind speed from a cup anemometer (interrupt on D2) |
| BS | Beaufort scale label from the wind speed |
| SHT-T, SHT-H | Temperature and humidity from an SHT21 (I2C 0x40) |
| **BMP pressure** | **Pressure from a BMP280 (I2C 0x76 or 0x77), the block in this repository** |
| BMP altitude | Altitude from the BMP280 (not in the final program) |
| CS | Communication system, data sent over RS485 with a MAX485 module |

## Timing and averaging

| Stage | What happens | Time |
|---|---|---|
| Sampling | Each sensor is read | every 10 s |
| First average | 15 samples are averaged | 2.5 min |
| Final average | 4 first averages are averaged | 10 min |

A LIVE packet is sent once at start-up, and a FINAL packet every 10 minutes.

## Data packet

Comma-separated text, one line per packet. This format is taken from the final transmitter program in `firmware/class_station_final`:

```
LIVE,28.5,45,1008,0.4,Calm
FINAL,28.3,46,1008,2.3,LightBreeze
```

| Field | Meaning |
|---|---|
| 1 | Packet type: LIVE or FINAL |
| 2 | Temperature in degC |
| 3 | Relative humidity in % |
| 4 | **Pressure in hPa, 0 decimals** |
| 5 | Wind speed in m/s |
| 6 | Beaufort label |

The pressure block prints its value in the same style (`LIVE,1008` and `FINAL,1008`), so it can be merged into this line.

## Installation

The finished station was installed on the rooftop of the ECE department. Its readings are shown on a small LCD in the HOD's cabin.

## What this repository covers

Our group's pressure block (`firmware/bmp280_pressure`) and the class's final transmitter program (`firmware/class_station_final`), for context. The receiver and LCD program is not included.
