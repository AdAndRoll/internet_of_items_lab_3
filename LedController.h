#pragma once

#include <Arduino.h>

// Управление штатным светодиодом на GPIO 2.
class LedController {
public:
  LedController(uint8_t pin, uint8_t pinMode);

  void begin();
  void update();

  bool getCurrentState() const;
  bool getPreviousState() const;

  void turnOn();
  void turnOff();

private:
  uint8_t _pin;
  uint8_t _pinMode;

  bool _currentState;
  bool _previousState;

  void applyState(bool state);
};
