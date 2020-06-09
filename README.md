# IoT Sourdough Starter Monitor

Internet connected sourdough starter monitor. Tracks fermentation with distance, temperature, and humidity sensors, and publishes data to the cloud.

<p align="center">
    <img src="docs/webapp.gif"><br>
    <i>Screencap of web app</i>
</p>


<p align="center">
    <img src="docs/kicad.png"><br>
    <i>PCB schematic and layout</i>
</p>


<p align="center">
    <img src="docs/pcb.png"><br>
    <i>Assembled PCB</i>
</p>

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

Versions:
- Arduino: 1.8.12
- ESP8266 core: 2.6.3
- ESP8266FS plugin: 0.5.0

Install the following libraries (Sketch > Include Library > Manage Libraries):

- Sensors
    - [DHT sensor library 1.3.8](https://github.com/adafruit/DHT-sensor-library)
    - [Adafruit_VL6180X 1.0.7](https://github.com/adafruit/Adafruit_VL6180X)
    - [Adafruit SSD1306 2.2.1](https://github.com/adafruit/Adafruit_SSD1306)
- AWS IOT:
    - [PubSubClient 2.7.0](https://github.com/knolleary/pubsubclient)
    - [ArduinoJson 6.15.1](https://github.com/bblanchon/ArduinoJson)

## AWS IoT Certificates

Convert the certificates from AWS to `.der` files, then place them in `data/` (within the Arduino sketch folder). Use the [arduino-esp8266fs-plugin](https://github.com/esp8266/arduino-esp8266fs-plugin) to upload the files to the SPIFFS filesystem.

```bash
openssl x509 -in xxx-certificate.pem.crt -out cert.der -outform DER
openssl rsa -in xxx-private.pem.key -out private.der -outform DER
openssl x509 -in AmazonRootCA1.pem -out ca.der -outform DER
```
