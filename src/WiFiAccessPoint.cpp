#include "WiFiAccessPoint.h"
#include "config.h"
#include <WiFi.h>

WiFiAccessPoint::WiFiAccessPoint(const char* ssid, const char* password)
    : _ssid(ssid), _password(password), _currentState(false), _previousState(false), _ipAddress{0} {}

bool WiFiAccessPoint::begin() {
  _previousState = _currentState;
  _ipAddress[0] = '\0';

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  delay(100);
  WiFi.begin(_ssid, _password);

  Serial.print(F("[WiFi] Подключение к "));
  Serial.println(_ssid);

  const unsigned long startMs = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startMs < WIFI_CONNECT_TIMEOUT_MS) {
    delay(500);
    Serial.print('.');
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("[WiFi] Не удалось подключиться"));
    _currentState = false;
    return false;
  }

  snprintf(_ipAddress, sizeof(_ipAddress), "%s", WiFi.localIP().toString().c_str());
  _currentState = true;

  Serial.print(F("[WiFi] Подключено, IP: "));
  Serial.println(_ipAddress);

  return true;
}

void WiFiAccessPoint::update() {
  _previousState = _currentState;
  _currentState = WiFi.status() == WL_CONNECTED;

  if (_currentState) {
    snprintf(_ipAddress, sizeof(_ipAddress), "%s", WiFi.localIP().toString().c_str());
  }
}

bool WiFiAccessPoint::getCurrentState() const {
  return _currentState;
}

const char* WiFiAccessPoint::getSsid() const {
  return _ssid;
}

const char* WiFiAccessPoint::getIpAddress() const {
  return _ipAddress;
}
