#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <BH1750.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


struct SensorData {
  float temperature;
  float humidity;
  float pressure;
  float light;
  int gas;
  bool valid;
};
Adafruit_BME280 bme;
BH1750 lightMeter;

enum SystemState {
  STATE_INIT,
  STATE_OK,
  STATE_WARNING,
  STATE_ERROR
};

volatile SystemState systemState = STATE_INIT;



volatile SensorData sensorData;

// -------------------
// Hardware definition
// --------------------
#define LED_PIN 2

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


// --------------------
// Task handles
// --------------------
TaskHandle_t systemTaskHandle = NULL;
TaskHandle_t sensorTaskHandle = NULL;
TaskHandle_t outputTaskHandle = NULL;

const float TEMP_MAX_WARNING = 35.0;
const float TEMP_MAX_ERROR   = 50.0;

const int   GAS_MAX_WARNING  = 2500;
const int   GAS_MAX_ERROR    = 3500;


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
  

  bool bmeOk = bme.begin(0x76) || bme.begin(0x77);
  bool lightOk = lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);

  while (true) {
    if (bmeOk && lightOk) {
      sensorData.temperature = bme.readTemperature();
      sensorData.humidity    = bme.readHumidity();
      sensorData.pressure    = bme.readPressure() / 100.0f;
      sensorData.light       = lightMeter.readLightLevel();
      sensorData.gas         = analogRead(34);
      sensorData.valid       = true;
    } else {
      sensorData.valid = false;
    }

    vTaskDelay(pdMS_TO_TICKS(4000)); // sensor task runs every 4s
  }
}


// --------------------
// Output Task
// --------------------
void outputTask(void *parameter) {

  // OLED init (only once)
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    systemState = STATE_ERROR;
    vTaskDelete(NULL);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  while (true) {

    display.clearDisplay();

    // Line 1: System State
    display.setCursor(0, 0);
    display.print("State: ");

    if (systemState == STATE_OK) display.print("OK");
    else if (systemState == STATE_WARNING) display.print("WARN");
    else if (systemState == STATE_ERROR) display.print("ERROR");
    else display.print("INIT");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("OLED OK");
    display.display();
vTaskDelay(pdMS_TO_TICKS(2000));


    // Line 2: Temperature
    display.setCursor(0, 16);
    display.print("Temp: ");
    display.print(sensorData.temperature);
    display.print(" C");

    // Line 3: Humidity
    display.setCursor(0, 26);
    display.print("Hum: ");
    display.print(sensorData.humidity);
    display.print(" %");

    // Line 4: Light
    display.setCursor(0, 36);
    display.print("Light: ");
    display.print(sensorData.light);

    // Line 5: Gas
    display.setCursor(0, 46);
    display.print("Gas: ");
    display.print(sensorData.gas);

    display.display();

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}


// --------------------
// Arduino setup
// --------------------
void setup() {
  Wire.begin(21, 22);

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
