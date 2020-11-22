#include <TaskScheduler.h>

#include "display.h"
#include "measurements.h"
#include "iot.h"

// If false, device will be standalone/offline
#define ENABLE_IOT false

Task tMeasure(100, TASK_FOREVER, &tMeasureCallback);
Task tDisplay(250, TASK_FOREVER, &tDisplayCallback);

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

  taskManager.init();
  taskManager.addTask(tMeasure);
  taskManager.addTask(tDisplay);
  tMeasure.enable();
  tDisplay.enable();

#if ENABLE_IOT
  initializeIoT();
  taskManager.addTask(tIoT);
  tIoT.enable();
#endif
}

void loop() {
  taskManager.execute();
}
