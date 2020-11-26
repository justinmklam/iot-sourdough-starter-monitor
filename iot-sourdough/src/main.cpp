#include <TaskScheduler.h>

#include "display.h"
#include "measurements.h"
#include "userinput.h"
#include "iot.h"

// If false, device will be standalone/offline
#define ENABLE_IOT false

Task tMeasure(100, TASK_FOREVER, &tMeasureCallback);
Task tDisplay(250, TASK_FOREVER, &tDisplayCallback);
Task tUserInput(25, TASK_FOREVER, &tUserInputCallback);

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
  taskManager.addTask(tUserInput);
  tMeasure.enable();
  tDisplay.enable();
  tUserInput.enable();

#if ENABLE_IOT
  initializeIoT();
  taskManager.addTask(tIoT);
  tIoT.enable();
#endif
}

void loop() {
  taskManager.execute();
}
