#include "LedController.h"
#include "config.h"

LedController::LedController(uint8_t pin, uint8_t pinMode)
    : _pin(pin), _pinMode(pinMode), _currentState(false), _previousState(false) {}

void LedController::begin() {
#if USE_HARDWARE
  pinMode(_pin, _pinMode);
#endif
  applyState(false);
}

void LedController::update() {
  _previousState = _currentState;
}

bool LedController::getCurrentState() const {
  return _currentState;
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
