# OLED HELLO example

## OLED development update — November 11, 2026

During today's development session, we tested the OLED display with the ESP32 and investigated display initialization, wiring, and upload issues. The OLED successfully displayed **HELLO** using a simple standalone test sketch, confirming that basic text output is achievable with the tested configuration.

The successful test used the U8g2 library with the following settings.

| Setting | Value |
| --- | --- |
| Arduino IDE board profile | ESP32 Dev Module |
| Display driver | SSD1309, 128 × 64 pixels |
| Interface | Software I²C |
| SDA | GPIO21 |
| SCL | GPIO22 |
| I²C address | `0x3C` |

The complete test sketch is available in [OLEDHello.ino](OLEDHello.ino). Install **U8g2** through Arduino IDE's Library Manager, open the example as its own sketch, select **ESP32 Dev Module** and the connected USB serial port, and upload it. The expected output is **HELLO** on the OLED.

We also attempted to run the existing OLED battery and speedometer code on the ESP32. That interface did not display as expected during testing and requires further edits. Work included reviewing the display driver and I²C settings, addressing missing `setup()` and `loop()` functions in standalone sketches, and examining how the existing drawing functions update the display buffer.

Further work is needed to adapt the legacy OLED interface to the tested screen configuration and verify reliable startup and display updates. Intermittent blank output and upload connection issues were also observed, so repeatable operation still needs to be confirmed before integrating the full bike interface.
