#include "WiFiAccessPoint.h"
#include <WiFi.h>

WiFiAccessPoint::WiFiAccessPoint(const char* ssid, const char* password)
    : _ssid(ssid),
      _password(password),
      _currentState(false),
      _previousState(false),
      _ipAddress{0} {}

bool WiFiAccessPoint::begin() {
  _previousState = _currentState;

  WiFi.mode(WIFI_AP);
  const bool started = WiFi.softAP(_ssid, _password);

  if (started) {
    const IPAddress ip = WiFi.softAPIP();
    snprintf(_ipAddress, sizeof(_ipAddress), "%s", ip.toString().c_str());
    _currentState = true;
  } else {
    _ipAddress[0] = '\0';
    _currentState = false;
  }

  return _currentState;
}

void WiFiAccessPoint::update() {
  _previousState = _currentState;
  _currentState = WiFi.softAPgetStationNum() >= 0 && WiFi.getMode() == WIFI_AP;
}

bool WiFiAccessPoint::getCurrentState() const {
  return _currentState;
}

bool WiFiAccessPoint::getPreviousState() const {
  return _previousState;
}

const char* WiFiAccessPoint::getSsid() const {
  return _ssid;
}

const char* WiFiAccessPoint::getIpAddress() const {
  return _ipAddress;
}

uint8_t WiFiAccessPoint::getClientCount() const {
  return WiFi.softAPgetStationNum();
}
