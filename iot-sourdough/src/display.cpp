#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "display.h"
#include "measurements.h"

Adafruit_SSD1306 display = Adafruit_SSD1306();

extern Measurements measurements;

void initializeDisplay() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.display();

  // text display big!
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Starting...");
  display.display();
}

void tDisplayCallback() {
    display.clearDisplay();
    display.setCursor(0,0);

    if (measurements.status == 0) {
      // Serial.print("Range: "); Serial.println(range);
      display.print(measurements.range);
      display.print("mm\n");
    } else {
      display.print("n/a\n");
      // Serial.print("Error reading VL6180X, code: ");
      // Serial.println(status);
      // return;
    }

    display.print(measurements.temperature);
    display.print("C\n");
    display.print(measurements.humidity);
    display.print("%");
    display.display();
}

