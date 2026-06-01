#pragma once

#include <Arduino.h>

// Точка доступа Wi-Fi на ESP32.
class WiFiAccessPoint {
public:
  WiFiAccessPoint(const char* ssid, const char* password);

  bool begin();
  void update();

  bool getCurrentState() const;
  bool getPreviousState() const;

  const char* getSsid() const;
  const char* getIpAddress() const;
  uint8_t getClientCount() const;

private:
  const char* _ssid;
  const char* _password;

  bool _currentState;
  bool _previousState;

  char _ipAddress[16];
};
