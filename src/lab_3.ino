#include "config.h"
#include "Aht20Sensor.h"
#include "CyclicDataBuffer.h"
#include "LedController.h"
#include "WebServerApp.h"
#include "WiFiAccessPoint.h"
#include <time.h>

WiFiAccessPoint wifiNetwork(WIFI_SSID, WIFI_PASSWORD);
LedController led(LED_PIN, LED_PIN_MODE);
Aht20Sensor aht20Sensor(I2C_SDA_PIN, I2C_SCL_PIN, AHT20_I2C_ADDRESS);
CyclicDataBuffer temperatureBuffer;
CyclicDataBuffer humidityBuffer;
WebServerApp webApp(wifiNetwork, led, temperatureBuffer, humidityBuffer);

unsigned long lastDataUpdateMs = 0;
unsigned long lastSensorRetryMs = 0;
bool sensorReady = false;

uint32_t currentTimestampSec() {
  time_t now = 0;
  time(&now);
  if (now > 1700000000UL) {
    return static_cast<uint32_t>(now);
  }
  return static_cast<uint32_t>(millis() / 1000UL);
}

void initTimeFromNtp() {
  configTime(TIME_GMT_OFFSET_SEC, TIME_DAYLIGHT_OFFSET_SEC, "pool.ntp.org", "time.google.com");

  struct tm timeinfo;
  for (uint8_t i = 0; i < 20; ++i) {
    if (getLocalTime(&timeinfo)) {
      Serial.println(F("[OK] Время синхронизировано (NTP)"));
      return;
    }
    delay(500);
  }
  Serial.println(F("[WARN] NTP недоступно — метки времени отсчитываются от старта ESP"));
}

void pushSensorToBuffers() {
  const uint32_t ts = currentTimestampSec();
  aht20Sensor.update();

  if (aht20Sensor.getCurrentState().valid) {
    sensorReady = true;
    temperatureBuffer.pushSample(aht20Sensor.getCurrentTemperature(), ts);
    humidityBuffer.pushSample(aht20Sensor.getCurrentHumidity(), ts);
    return;
  }

  sensorReady = false;
  static float simTemp = SIM_INITIAL_TEMPERATURE;
  static float simHum = SIM_INITIAL_HUMIDITY;
  simTemp += 0.05f;
  simHum += 0.15f;
  temperatureBuffer.pushSample(simTemp, ts);
  humidityBuffer.pushSample(simHum, ts);
}

void setup() {
  Serial.begin(115200);
  randomSeed(esp_random());
  delay(500);

  led.begin();
  temperatureBuffer.begin();
  humidityBuffer.begin();

  while (!wifiNetwork.begin()) {
    Serial.println(F("--- Повтор Wi-Fi через 5 с (включите раздачу на телефоне) ---"));
    delay(5000);
  }

  initTimeFromNtp();

  sensorReady = aht20Sensor.begin();
  if (!sensorReady) {
    Serial.println(F("[ERR] AHT20 не найден — буфер заполнится тестовыми значениями"));
#if USE_HARDWARE
    Aht20Sensor::scanI2CBus(I2C_SDA_PIN, I2C_SCL_PIN);
#endif
  } else {
    Serial.println(F("[OK] AHT20 инициализирован"));
  }
  pushSensorToBuffers();

  if (!webApp.begin()) {
    Serial.println(F("Ошибка запуска web-сервера"));
    while (true) {
      delay(1000);
    }
  }

  lastDataUpdateMs = millis();

  Serial.println(F("=== Lab 3 готов ==="));
  Serial.print(F("Сеть: "));
  Serial.println(WIFI_SSID);
  Serial.print(F("Откройте в браузере: http://"));
  Serial.println(wifiNetwork.getIpAddress());
  Serial.print(F("Логин: "));
  Serial.print(AUTH_LOGIN);
  Serial.print(F(" / "));
  Serial.println(AUTH_PASSWORD);
}

void loop() {
  wifiNetwork.update();
  led.update();
  webApp.update();

  const unsigned long now = millis();

  if (!sensorReady && now - lastSensorRetryMs >= 10000) {
    lastSensorRetryMs = now;
    sensorReady = aht20Sensor.begin();
    if (sensorReady) {
      Serial.println(F("[OK] AHT20 подключён повторно"));
    }
  }

  if (now - lastDataUpdateMs >= DATA_UPDATE_INTERVAL_MS) {
    lastDataUpdateMs = now;
    pushSensorToBuffers();
  }
}
