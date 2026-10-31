# Known issues in the class transmitter program

Found while reading `firmware/class_station_final/class_station_final.ino`. None of them stopped the station from working, but they are worth fixing in a next version.

| # | Issue | Suggested fix |
|---|---|---|
| 1 | `(Wire.read() << 8) \| Wire.read()` reads two bytes in one expression, and C++ does not define which one is evaluated first, so the high and low bytes can swap on some compilers | Read each byte into its own variable, then combine them |
| 2 | The first LIVE packet computes wind as `pulses / 2.0`, but there is no 2-second measuring window, so the first wind value is not meaningful | Send 0 for wind in LIVE, or count pulses for a fixed window |
| 3 | The two status bits at the end of each raw SHT21 value are not masked | `raw &= 0xFFFC;` before converting |
| 4 | The reed switch may bounce and give extra pulses | Ignore pulses that arrive within a few milliseconds of the last one |
| 5 | If an SHT21 read fails, temperature and humidity stay at 0 and are still averaged | Skip storing the sample when `readSHT21` returns false |
| 6 | `pulseCount` is a signed `int`, and a very high pulse rate could overflow it | Use `unsigned int` or `unsigned long` |
| 7 | `String` objects can fragment memory in a program that runs for months | Use `const char*` for the Beaufort labels |
| 8 | If the BMP280 is not found the program prints `BMP FAIL` and stops, so nothing is sent over RS485 | Keep running, and send the other values with a placeholder for pressure |
| 9 | The BMP280 altitude block is not implemented | Add `bmp.readAltitude(sea_level_hPa)` and include it in the packet |

The PC simulations in `firmware/tests` avoid issue 1 by using raw values whose two bytes are equal.
