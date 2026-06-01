#pragma once

#include <Arduino.h>
#include <functional>
#include "CyclicDataBuffer.h"
#include "LedController.h"
#include "WiFiAccessPoint.h"

// Асинхронный web-сервер: аутентификация, LED, API данных.
class WebServerApp {
public:
  WebServerApp(WiFiAccessPoint& accessPoint,
                 LedController& led,
                 CyclicDataBuffer& dataBuffer,
                 uint16_t port = 80);

  bool begin();

  void update();

  bool getCurrentState() const;
  bool getPreviousState() const;

private:
  WiFiAccessPoint& _accessPoint;
  LedController& _led;
  CyclicDataBuffer& _dataBuffer;
  uint16_t _port;

  bool _currentState;
  bool _previousState;

  class AsyncWebServer* _server;
  String _sessionToken;

  void registerRoutes();
  bool isAuthenticated(class AsyncWebServerRequest* request) const;
  void requireAuth(class AsyncWebServerRequest* request,
                   std::function<void()> onSuccess) const;
  String readSessionFromCookie(class AsyncWebServerRequest* request) const;
  void createSession();
  void clearSession();
};
