# IoT Sourdough Starter Monitor

Internet connected sourdough starter monitor. Tracks fermentation with distance, temperature, and humidity sensors, and publishes data to the cloud.

## Hardware

- NodeMCU ESP8266 (aka NodeMCU 1.0 ESP-12E)
- DHT22 Temperature and humidity sensor
- VL6180X Time of flight distance sensor
- SSD1306 Monochrome 128x32 I2C OLED display

For the DHT22:

| Pinout | Connection |
|---|---|
| Vcc | 3v3 |
| Data | SD3 (GPIO 10) w/ 10K pullup resistor |
| NC | - |
| GND | GND |

For the VL6180X and SSD1306:

| Pinout | Connection |
|---|---|
| Vcc | 3v3 |
| SCL | D1 (GPIO 5) |
| SDA | D2 (GPIO 4) |
| GND | GND |

## Getting Started

Add the ESP8266 board to Arduino IDE (Files > Preferences, then paste the URL in `Additional Boards Manager`):

```
http://arduino.esp8266.com/stable/package_esp8266com_index.json
```

Install the following libraries (Sketch > Include Library > Manage Libraries):

- [DHT sensor library](https://github.com/adafruit/DHT-sensor-library)
- [Adafruit_VL6180X](https://github.com/adafruit/Adafruit_VL6180X)
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
