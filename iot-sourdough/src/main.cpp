#include <TaskScheduler.h>

#include "display.h"
#include "measurements.h"
#include "button.h"
#include "iot.h"

// If false, device will be standalone/offline
#define ENABLE_IOT false

Task tMeasure(100, TASK_FOREVER, &tMeasureCallback);
Task tDisplay(250, TASK_FOREVER, &tDisplayCallback);
Task tButton(100, TASK_FOREVER, &tButtonCallback);

#if ENABLE_IOT
Task tIoT(5000, TASK_FOREVER, &tIoTCallback);
#endif

Scheduler taskManager;

void setup()
{
  Serial.begin(115200);
  Serial.println("AWS Levain Monitor");
  delay(100);

  initializeMeasurements();
  initializeDisplay();
  initializeButton();

  taskManager.init();
  taskManager.addTask(tMeasure);
  taskManager.addTask(tDisplay);
  taskManager.addTask(tButton);
  tMeasure.enable();
  tDisplay.enable();
  tButton.enable();

#if ENABLE_IOT
  initializeIoT();
  taskManager.addTask(tIoT);
  tIoT.enable();
#endif
}

void loop() {
  taskManager.execute();
}
