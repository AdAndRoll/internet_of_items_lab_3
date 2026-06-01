#include "WebServerApp.h"

#include <ESPAsyncWebServer.h>
#include <functional>

#include "WebPages.h"
#include "config.h"

namespace {
AsyncWebServer* g_server = nullptr;
String g_sessionToken;
}  // namespace

WebServerApp::WebServerApp(WiFiAccessPoint& accessPoint,
                           LedController& led,
                           CyclicDataBuffer& dataBuffer,
                           uint16_t port)
    : _accessPoint(accessPoint),
      _led(led),
      _dataBuffer(dataBuffer),
      _port(port),
      _currentState(false),
      _previousState(false),
      _server(nullptr) {}

bool WebServerApp::begin() {
  _previousState = _currentState;

  if (_server != nullptr) {
    delete _server;
  }

  _server = new AsyncWebServer(_port);
  g_server = _server;
  g_sessionToken = _sessionToken;

  registerRoutes();
  _server->begin();

  _currentState = true;
  return true;
}

void WebServerApp::update() {
  _previousState = _currentState;
  _currentState = _server != nullptr;
}

bool WebServerApp::getCurrentState() const {
  return _currentState;
}

bool WebServerApp::getPreviousState() const {
  return _previousState;
}

void WebServerApp::registerRoutes() {
  _server->on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    if (g_sessionToken.length() > 0 &&
        request->hasHeader("Cookie") &&
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
    requireAuth(request, [request]() {
      request->send(200, "text/html; charset=utf-8", WebPages::ledPage());
    });
  });

  _server->on("/data", HTTP_GET, [this](AsyncWebServerRequest* request) {
    requireAuth(request, [request]() {
      request->send(200, "text/html; charset=utf-8", WebPages::dataPage());
    });
  });

  _server->on("/api/led/state", HTTP_GET, [this](AsyncWebServerRequest* request) {
    requireAuth(request, [this, request]() {
      const String json = String(F("{\"on\":")) + (_led.getCurrentState() ? "true" : "false") + "}";
      request->send(200, "application/json", json);
    });
  });

  _server->on("/api/led/on", HTTP_POST, [this](AsyncWebServerRequest* request) {
    requireAuth(request, [this, request]() {
      _led.turnOn();
      _led.update();
      request->send(200, "application/json", F("{\"ok\":true,\"on\":true}"));
    });
  });

  _server->on("/api/led/off", HTTP_POST, [this](AsyncWebServerRequest* request) {
    requireAuth(request, [this, request]() {
      _led.turnOff();
      _led.update();
      request->send(200, "application/json", F("{\"ok\":true,\"on\":false}"));
    });
  });

  _server->on("/api/data", HTTP_GET, [this](AsyncWebServerRequest* request) {
    requireAuth(request, [this, request]() {
      const String json = String(F("{\"values\":")) + _dataBuffer.toJsonArray() + "}";
      request->send(200, "application/json", json);
    });
  });

  _server->onNotFound([](AsyncWebServerRequest* request) {
    request->redirect("/");
  });
}

bool WebServerApp::isAuthenticated(AsyncWebServerRequest* request) const {
  if (_sessionToken.length() == 0) {
    return false;
  }

  const String cookieValue = readSessionFromCookie(request);
  return cookieValue == _sessionToken;
}

void WebServerApp::requireAuth(AsyncWebServerRequest* request,
                               std::function<void()> onSuccess) const {
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

void WebServerApp::clearSession() {
  _sessionToken = "";
  g_sessionToken = "";
}
