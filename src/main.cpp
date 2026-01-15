#include <Arduino.h>

// --------------------
// Hardware definition
// --------------------
#define LED_PIN 2

// --------------------
// Task handles
// --------------------
TaskHandle_t systemTaskHandle = NULL;
TaskHandle_t sensorTaskHandle = NULL;
TaskHandle_t outputTaskHandle = NULL;

// --------------------
// System Task (Heartbeat)
// --------------------
void systemTask(void *parameter) {
  pinMode(LED_PIN, OUTPUT);

  while (true) {
    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(100));
    digitalWrite(LED_PIN, LOW);

    vTaskDelay(pdMS_TO_TICKS(900)); // total ~1 second
  }
}

// --------------------
// Sensor Task (Dummy)
// --------------------
void sensorTask(void *parameter) {
  while (true) {
    // Double blink
    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(100));
    digitalWrite(LED_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(100));

    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(100));
    digitalWrite(LED_PIN, LOW);

    vTaskDelay(pdMS_TO_TICKS(3600)); // total ~4 seconds
  }
}

// --------------------
// Output Task
// --------------------
void outputTask(void *parameter) {
  while (true) {
    // Long blink
    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(500));
    digitalWrite(LED_PIN, LOW);

    vTaskDelay(pdMS_TO_TICKS(1500)); // total ~2 seconds
  }
}

// --------------------
// Arduino setup
// --------------------
void setup() {
  pinMode(LED_PIN, OUTPUT);

  xTaskCreate(
    systemTask,
    "System Task",
    2048,
    NULL,
    1,
    &systemTaskHandle
  );

  xTaskCreate(
    sensorTask,
    "Sensor Task",
    2048,
    NULL,
    1,
    &sensorTaskHandle
  );

  xTaskCreate(
    outputTask,
    "Output Task",
    2048,
    NULL,
    1,
    &outputTaskHandle
  );
}

// --------------------
// Arduino loop
// --------------------
void loop() {
  // Empty on purpose.
  // RTOS scheduler is running the system.
}
