#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "display.h"
#include "iot.h"
#include "measurements.h"
#include "userinput.h"

#define SSD1306_WIDTH_PX 128
#define SSD1306_HEIGHT_PX 32

int displayState = DISPLAY_STATE_DEFAULT;

Adafruit_SSD1306 display = Adafruit_SSD1306(SSD1306_WIDTH_PX, SSD1306_HEIGHT_PX);
extern CircularBuffer<float, SSD1306_WIDTH_PX> bufferRiseHeight;

extern Measurements measurements;
extern bool wifi_connected;

void initializeDisplay()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3C (for the 128x32)
  display.display();

  // text display big!
  display.setTextSize(2);
  display.setTextColor(WHITE);

  display.clearDisplay();
  display.setCursor(8, 0);
  display.println("SOURDOUGH");
  display.setCursor(20, SSD1306_HEIGHT_PX / 2);
  display.println("MONITOR");
  display.display();
}

void setNextDisplayState()
{
  displayState++;

  if (displayState == NUM_DISPLAY_STATES)
  {
    displayState = 0;
  }
}

float calculateHeightToPlot(float height, float maxHeight)
{
  // Need to invert since display/plotting origin is top left
  return SSD1306_HEIGHT_PX - height / maxHeight * SSD1306_HEIGHT_PX;
}

void tDisplayCallback()
{
  switch (getState())
  {
  case STATE_CALIBRATION:
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);

    display.println("Calibration");
    display.print("Jar height: ");
    display.print(measurements.range);
    display.print(" mm");
    break;

  default:
    // Show the different "dashboards"
    switch (displayState)
    {
    case DISPLAY_STATE_DEFAULT:
      display.clearDisplay();
      display.setCursor(25, 0);
      display.setTextSize(3);
      display.print(measurements.maxRisePercent, 0);
      display.println("%");
      display.setTextSize(1);
      display.print(" Peaked ");
      display.print(measurements.timeSinceMaxRiseMins, 0);
      display.print(" mins ago");

      if (measurements.isMonitoring == true)
      {
        // Show "recording" in top right corner
        display.fillCircle(123, 3, 2, WHITE);
      }

      if (wifi_connected == false)
      {
        // Show "x" if not connected to wifi
        display.setCursor(0,0);
        display.setTextSize(1);
        display.print("x");
      }

      break;

    case DISPLAY_STATE_GRAPH:
      static int x0 = 0;
      static int y0 = SSD1306_HEIGHT_PX;
      ;
      static int y1 = y0;
      static float maxRiseHeight;
      static float riseDurationMs = 0;

      display.clearDisplay();

      // Reset ymax in the plot
      if (measurements.rise_percent == 0)
      {
        maxRiseHeight = 50;
      }

      // Get max so plot autoscales in vertical axis
      for (int i = 0; i < bufferRiseHeight.size(); i++)
      {
        if (bufferRiseHeight[i] > maxRiseHeight)
        {
          maxRiseHeight = bufferRiseHeight[i];
        }
      }

      x0 = 0;
      y0 = calculateHeightToPlot(bufferRiseHeight[0], maxRiseHeight);

      for (int i = 1; i < bufferRiseHeight.size(); i++)
      {
        y1 = calculateHeightToPlot(bufferRiseHeight[i], maxRiseHeight);
        display.writeLine(x0, y0, i, y1, WHITE);

        x0 = i;
        y0 = y1;
      }

      riseDurationMs = measurements.timeOfMaxRiseMs - measurements.sessionStartTimeMs;
      // Show rise time at bottom right of screen
      display.setCursor(SSD1306_WIDTH_PX - 40, SSD1306_HEIGHT_PX - 7);
      display.setTextSize(1);
      char graph_text[10];
      snprintf(graph_text, sizeof(graph_text),
               "%4.1f h", riseDurationMs / 1000 / 3600);
      display.println(graph_text);

      break;

    case DISPLAY_STATE_ADVANCED:
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(1);
      char adv_row0[128];
      char adv_row1[128];
      char adv_row2[128];
      char adv_row3[128];

      snprintf(adv_row0, sizeof(adv_row0),
               "M: %5.1f %%, %.1f min", measurements.maxRisePercent, measurements.timeSinceMaxRiseMins);
      snprintf(adv_row1, sizeof(adv_row1),
               "R: %5.1f %%  T: %.1f C", measurements.rise_percent, measurements.temperature);
      snprintf(adv_row2, sizeof(adv_row2),
               "R: %4d mm  H: %.1f %%", measurements.rise_height, measurements.humidity);
      snprintf(adv_row3, sizeof(adv_row3),
               "D: %4d mm  J: %3d mm", measurements.range, measurements.jarHeightMm);
      display.println(adv_row0);
      display.println(adv_row1);
      display.println(adv_row2);
      display.println(adv_row3);
      break;
    }
    break;
  }
  display.display();
}
