#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "display.h"
#include "measurements.h"
#include "userinput.h"

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

  switch (getState()) {
    case STATE_CALIBRATION:
      display.println("Calibration");
      display.print("Jar height: ");
      display.print(measurements.range);
      display.print(" mm");
      break;

    default:
      // Show the different "dashboards"
      switch (getDisplayState()) {
        case DISPLAY_STATE_DEFAULT:
          if (measurements.status == 0) {
            display.print(measurements.rise_percent);
            display.print("%\n");
          } else {
            display.print("n/a\n");
          }

          display.print(measurements.temperature);
          display.print("C\n");
          display.print(measurements.humidity);
          display.print("%\n");
          break;

        case DISPLAY_STATE_GRAPH:
          display.println("graph");
          break;

        case DISPLAY_STATE_ADVANCED:
          char adv_row1[128];
          char adv_row2[128];
          char adv_row3[128];

          snprintf(adv_row1, sizeof(adv_row1),
            "R: %5.1f %%  T: %.1f C", measurements.rise_percent, measurements.temperature
          );
          snprintf(adv_row2, sizeof(adv_row2),
            "R: %4d mm  H: %.1f %%", measurements.rise_height, measurements.humidity
          );
          snprintf(adv_row3, sizeof(adv_row3),
            "D: %4d mm", measurements.range
          );
          display.println(adv_row1);
          display.println(adv_row2);
          display.println(adv_row3);
          break;
      }
      break;
  }
  display.display();
}
