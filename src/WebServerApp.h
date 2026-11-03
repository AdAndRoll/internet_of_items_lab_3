#pragma once

#include <Arduino.h>
#include <functional>
#include "CyclicDataBuffer.h"
#include "LedController.h"
#include "WiFiAccessPoint.h"

class WebServerApp {
public:
  WebServerApp(WiFiAccessPoint& accessPoint,
               LedController& led,
               CyclicDataBuffer& temperatureBuffer,
               CyclicDataBuffer& humidityBuffer,
               uint16_t port = 80);

  bool begin();
  void update();

private:
  WiFiAccessPoint& _accessPoint;
  LedController& _led;
  CyclicDataBuffer& _temperatureBuffer;
  CyclicDataBuffer& _humidityBuffer;
  uint16_t _port;

  class AsyncWebServer* _server;
  String _sessionToken;

  void registerRoutes();
  bool isAuthenticated(class AsyncWebServerRequest* request) const;
  void requireAuth(class AsyncWebServerRequest* request, std::function<void()> onSuccess) const;
  String readSessionFromCookie(class AsyncWebServerRequest* request) const;
  void createSession();
};
