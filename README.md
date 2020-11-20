# IoT Sourdough Starter Monitor

Internet connected sourdough starter monitor. Tracks fermentation with distance, temperature, and humidity sensors, and publishes data to the cloud.

<p align="center">
    <img src="docs/webapp.gif"><br>
    <i>Web app demo with dummy data.</i>
</p>


<p align="center">
    <img src="docs/kicad.png"><br>
    <i>PCB schematic and layout in KiCad.</i>
</p>


<p align="center">
    <img src="docs/pcb.png"><br>
    <i>Assembled PCB.</i>
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

## Software

AWS is used as the cloud backend. Data flow is as follows:

- ESP8266 sends sensor data over MQTT
- Kinesis Firehose receives data
- Lambda function puts data from Firehose to S3 on data receive event
- Web app queries S3 data via Athena


## Getting Started

### PlatformIO

This project is built using PlatformIO. To build:

```bash
# Build
pio run

# Build and upload
pio run -t upload
```

To view the serial monitor:

```bash
pio device monitor -b 115200
```

Dependencies (specified in `platformio.ini`):

- Sensors
    - [DHT sensor library 1.3.8](https://github.com/adafruit/DHT-sensor-library)
    - [Adafruit_VL6180X 1.0.7](https://github.com/adafruit/Adafruit_VL6180X)
    - [Adafruit SSD1306 2.2.1](https://github.com/adafruit/Adafruit_SSD1306)
- AWS IOT:
    - [PubSubClient 2.7.0](https://github.com/knolleary/pubsubclient)
    - [ArduinoJson 6.15.1](https://github.com/bblanchon/ArduinoJson)

## AWS IoT Certificates

Convert the certificates from AWS to `.der` files, then place them in `data/` (within the project directory).

```bash
openssl x509 -in xxx-certificate.pem.crt -out cert.der -outform DER
openssl rsa -in xxx-private.pem.key -out private.der -outform DER
openssl x509 -in AmazonRootCA1.pem -out ca.der -outform DER
```

Use the following command to upload the files to the SPIFFS filesystem:

```bash
pio run -t uploadfs
```

Alternatively, you may use the [arduino-esp8266fs-plugin](https://github.com/esp8266/arduino-esp8266fs-plugin) to upload the files.
