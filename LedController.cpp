#include "LedController.h"
#include "config.h"

LedController::LedController(uint8_t pin, uint8_t pinMode)
    : _pin(pin),
      _pinMode(pinMode),
      _currentState(false),
      _previousState(false) {}

void LedController::begin() {
#if USE_HARDWARE
  pinMode(_pin, _pinMode);
  applyState(false);
#else
  Serial.println(F("[LED] init (simulation)"));
#endif
}

void LedController::update() {
  if (_currentState != _previousState) {
#if !USE_HARDWARE
    Serial.print(F("[LED] state: "));
    Serial.println(_currentState ? F("ON") : F("OFF"));
#endif
    _previousState = _currentState;
  }
}

bool LedController::getCurrentState() const {
  return _currentState;
}

bool LedController::getPreviousState() const {
  return _previousState;
}

void LedController::turnOn() {
  applyState(true);
}

void LedController::turnOff() {
  applyState(false);
}

void LedController::applyState(bool state) {
  _previousState = _currentState;
  _currentState = state;

#if USE_HARDWARE
  const bool pinLevel = LED_ACTIVE_LOW ? !state : state;
  digitalWrite(_pin, pinLevel ? HIGH : LOW);
#endif
}
