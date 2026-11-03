#include "WebServerApp.h"

#include <ESPAsyncWebServer.h>
#include <stdio.h>

#include "WebPages.h"
#include "config.h"

namespace {
AsyncWebServer* g_server = nullptr;
String g_sessionToken;
}  // namespace

WebServerApp::WebServerApp(WiFiAccessPoint& accessPoint,
                           LedController& led,
                           CyclicDataBuffer& temperatureBuffer,
                           CyclicDataBuffer& humidityBuffer,
                           uint16_t port)
    : _accessPoint(accessPoint),
      _led(led),
      _temperatureBuffer(temperatureBuffer),
      _humidityBuffer(humidityBuffer),
      _port(port),
      _server(nullptr) {}

bool WebServerApp::begin() {
  if (_server != nullptr) {
    delete _server;
  }

  _server = new AsyncWebServer(_port);
  g_server = _server;
  g_sessionToken = _sessionToken;

  registerRoutes();
  _server->begin();
  return true;
}

void WebServerApp::update() {
  (void)_accessPoint;
}

void WebServerApp::registerRoutes() {
  _server->on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    if (g_sessionToken.length() > 0 && request->hasHeader("Cookie") &&
        request->header("Cookie").indexOf(String(SESSION_COOKIE_NAME) + "=" + g_sessionToken) >= 0) {
      request->redirect("/led");
      return;
    }
    const bool showError = request->hasParam("error");
    request->send(200, "text/html; charset=utf-8", WebPages::loginPage(showError));
  });

  _server->on("/login", HTTP_POST, [this](AsyncWebServerRequest* request) {
    const String username = request->arg("username");
    const String password = request->arg("password");

    if (username == AUTH_LOGIN && password == AUTH_PASSWORD) {
      createSession();
      g_sessionToken = _sessionToken;

      AsyncWebServerResponse* response = request->beginResponse(302, "text/plain", "");
      response->addHeader("Location", "/led");
      response->addHeader("Set-Cookie",
                          String(SESSION_COOKIE_NAME) + "=" + _sessionToken + "; Path=/; HttpOnly");
      request->send(response);
      return;
    }

    request->redirect("/?error=1");
  });

  _server->on("/led", HTTP_GET, [this](AsyncWebServerRequest* request) {
    requireAuth(request, [request]() { request->send(200, "text/html; charset=utf-8", WebPages::ledPage()); });
  });

  _server->on("/data", HTTP_GET, [this](AsyncWebServerRequest* request) {
    requireAuth(request, [request]() { request->send(200, "text/html; charset=utf-8", WebPages::dataPage()); });
  });

  _server->on("/api/led/state", HTTP_GET, [this](AsyncWebServerRequest* request) {
    requireAuth(request, [this, request]() {
      request->send(200, "application/json",
                    String(F("{\"on\":")) + (_led.getCurrentState() ? "true" : "false") + "}");
    });
  });

  _server->on("/api/led/on", HTTP_POST, [this](AsyncWebServerRequest* request) {
    requireAuth(request, [this, request]() {
      _led.turnOn();
      request->send(200, "application/json", F("{\"ok\":true,\"on\":true}"));
    });
  });

  _server->on("/api/led/off", HTTP_POST, [this](AsyncWebServerRequest* request) {
    requireAuth(request, [this, request]() {
      _led.turnOff();
      request->send(200, "application/json", F("{\"ok\":true,\"on\":false}"));
    });
  });

  _server->on("/api/data", HTTP_GET, [this](AsyncWebServerRequest* request) {
    requireAuth(request, [this, request]() {
      String json = F("{\"temperature\":");
      json += _temperatureBuffer.toJsonArray();
      json += F(",\"humidity\":");
      json += _humidityBuffer.toJsonArray();
      json += F(",\"timestamps\":");
      json += _temperatureBuffer.toTimestampsJsonArray();
      json += F(",\"count\":");
      json += _temperatureBuffer.getCount();
      char num[16];
      json += F(",\"currentTemp\":");
      snprintf(num, sizeof(num), "%.2f", _temperatureBuffer.getCurrentValue());
      json += num;
      json += F(",\"currentHum\":");
      snprintf(num, sizeof(num), "%.2f", _humidityBuffer.getCurrentValue());
      json += num;
      json += '}';
      request->send(200, "application/json", json);
    });
  });

  _server->onNotFound([](AsyncWebServerRequest* request) { request->redirect("/"); });
}

bool WebServerApp::isAuthenticated(AsyncWebServerRequest* request) const {
  if (_sessionToken.length() == 0) {
    return false;
  }
  return readSessionFromCookie(request) == _sessionToken;
}

void WebServerApp::requireAuth(AsyncWebServerRequest* request, std::function<void()> onSuccess) const {
  if (!isAuthenticated(request)) {
    request->redirect("/");
    return;
  }
  onSuccess();
}

String WebServerApp::readSessionFromCookie(AsyncWebServerRequest* request) const {
  if (!request->hasHeader("Cookie")) {
    return "";
  }

  const String cookieHeader = request->header("Cookie");
  const String prefix = String(SESSION_COOKIE_NAME) + "=";
  const int start = cookieHeader.indexOf(prefix);
  if (start < 0) {
    return "";
  }

  const int valueStart = start + prefix.length();
  int valueEnd = cookieHeader.indexOf(';', valueStart);
  if (valueEnd < 0) {
    valueEnd = cookieHeader.length();
  }

  return cookieHeader.substring(valueStart, valueEnd);
}

void WebServerApp::createSession() {
  _sessionToken = String(random(0x100000, 0xFFFFFF), HEX);
}
