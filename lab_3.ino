#include "config.h"
#include "CyclicDataBuffer.h"
#include "LedController.h"
#include "WebServerApp.h"
#include "WiFiAccessPoint.h"

WiFiAccessPoint accessPoint(AP_SSID, AP_PASSWORD);
LedController led(LED_PIN, LED_PIN_MODE);
CyclicDataBuffer dataBuffer;
WebServerApp webApp(accessPoint, led, dataBuffer);

void setup() {
  Serial.begin(115200);

  if (!accessPoint.begin()) {
    Serial.println(F("Ошибка запуска точки доступа Wi-Fi"));
    while (true) {
      delay(1000);
    }
  }

  led.begin();
  dataBuffer.begin();

  if (!webApp.begin()) {
    Serial.println(F("Ошибка запуска web-сервера"));
    while (true) {
      delay(1000);
    }
  }

  Serial.println(F("=== Lab 3 готов ==="));
  Serial.print(F("Wi-Fi AP: "));
  Serial.println(accessPoint.getSsid());
  Serial.print(F("Пароль AP: "));
  Serial.println(AP_PASSWORD);
  Serial.print(F("IP сервера: http://"));
  Serial.println(accessPoint.getIpAddress());
  Serial.print(F("Логин: "));
  Serial.print(AUTH_LOGIN);
  Serial.print(F(" / Пароль: "));
  Serial.println(AUTH_PASSWORD);
}

void loop() {
  accessPoint.update();
  dataBuffer.update();
  led.update();
  webApp.update();
}
