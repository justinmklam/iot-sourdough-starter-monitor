#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "display.h"
#include "measurements.h"
#include "userinput.h"

#define SSD1306_WIDTH_PX 128
#define SSD1306_HEIGHT_PX 32

Adafruit_SSD1306 display = Adafruit_SSD1306(SSD1306_WIDTH_PX, SSD1306_HEIGHT_PX);

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
  static int prevDisplayState = DISPLAY_STATE_DEFAULT;

  switch (getState()) {
    case STATE_CALIBRATION:
      display.clearDisplay();
      display.setCursor(0,0);
      display.setTextSize(1);

      display.println("Calibration");
      display.print("Jar height: ");
      display.print(measurements.range);
      display.print(" mm");
      break;

    default:
      // Show the different "dashboards"
      switch (getDisplayState()) {
        case DISPLAY_STATE_DEFAULT:
          display.clearDisplay();
          display.setCursor(0,0);
          display.setTextSize(3);
          display.print(measurements.rise_percent);
          display.print("%");
          break;

        case DISPLAY_STATE_GRAPH:
          static int x0 = 0;
          static int y0 = SSD1306_HEIGHT_PX;;
          static int x1 = 0;
          static int y1 = y0;

          if (prevDisplayState != getDisplayState()) {
            display.clearDisplay();
            // Reset coordinates for now until we figure out array plotting
            x0 = 0;
            x1 = 0;
          }

          x1 = x0 + 1;
          y1 = SSD1306_HEIGHT_PX - measurements.rise_height;

          display.writeLine(x0, y0, x1, y1, WHITE);
          Serial.print(x0);
          Serial.print(",");
          Serial.println(y0);

          x0 = x1;
          y0 = y1;
          break;

        case DISPLAY_STATE_ADVANCED:
          display.clearDisplay();
          display.setCursor(0,0);
          display.setTextSize(1);
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

  prevDisplayState = getDisplayState();
}
