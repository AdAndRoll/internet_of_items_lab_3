#pragma once

#include <Arduino.h>

class WiFiAccessPoint {
public:
  WiFiAccessPoint(const char* ssid, const char* password);

  bool begin();
  void update();

  bool getCurrentState() const;
  const char* getSsid() const;
  const char* getIpAddress() const;

private:
  const char* _ssid;
  const char* _password;
  bool _currentState;
  bool _previousState;
  char _ipAddress[16];
};
