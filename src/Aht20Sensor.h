#pragma once

#include <Arduino.h>

struct Aht20Reading {
  float temperatureC;
  float humidityPct;
  bool valid;
};

class Aht20Sensor {
public:
  Aht20Sensor(uint8_t sdaPin, uint8_t sclPin, uint8_t i2cAddress);

  bool begin();
  void update();

  Aht20Reading getCurrentState() const;
  float getCurrentTemperature() const;
  float getCurrentHumidity() const;

  static void scanI2CBus(uint8_t sdaPin, uint8_t sclPin);

private:
  uint8_t _sdaPin;
  uint8_t _sclPin;
  uint8_t _i2cAddress;
  Aht20Reading _currentState;
  Aht20Reading _previousState;
  unsigned long _lastSimUpdateMs;

  bool readHardware();
  bool readSimulated();
};
