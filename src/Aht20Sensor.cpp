#include "Aht20Sensor.h"
#include "config.h"

#if USE_HARDWARE
#include <Wire.h>
#include <Adafruit_AHTX0.h>

Adafruit_AHTX0 aht20Chip;

namespace {
constexpr uint8_t kAhtAddresses[] = {0x38, 0x39};

bool tryInitAht(uint8_t sdaPin, uint8_t sclPin, uint8_t address) {
  Wire.begin(sdaPin, sclPin);
  Wire.setClock(100000);
  delay(100);
  return aht20Chip.begin(&Wire, 0, address);
}
}  // namespace
#endif

Aht20Sensor::Aht20Sensor(uint8_t sdaPin, uint8_t sclPin, uint8_t i2cAddress)
    : _sdaPin(sdaPin),
      _sclPin(sclPin),
      _i2cAddress(i2cAddress),
      _currentState{0.0f, 0.0f, false},
      _previousState{0.0f, 0.0f, false},
      _lastSimUpdateMs(0) {}

bool Aht20Sensor::begin() {
#if USE_HARDWARE
  for (uint8_t attempt = 0; attempt < 3; ++attempt) {
    if (tryInitAht(_sdaPin, _sclPin, _i2cAddress)) {
      update();
      return _currentState.valid;
    }
    delay(200);
  }
  for (uint8_t address : kAhtAddresses) {
    if (address != _i2cAddress && tryInitAht(_sdaPin, _sclPin, address)) {
      _i2cAddress = address;
      update();
      return _currentState.valid;
    }
  }
  return false;
#else
  _currentState = {SIM_INITIAL_TEMPERATURE, SIM_INITIAL_HUMIDITY, true};
  _previousState = _currentState;
  return true;
#endif
}

void Aht20Sensor::update() {
  _previousState = _currentState;
#if USE_HARDWARE
  readHardware();
#else
  readSimulated();
#endif
}

Aht20Reading Aht20Sensor::getCurrentState() const {
  return _currentState;
}

float Aht20Sensor::getCurrentTemperature() const {
  return _currentState.temperatureC;
}

float Aht20Sensor::getCurrentHumidity() const {
  return _currentState.humidityPct;
}

#if USE_HARDWARE
void Aht20Sensor::scanI2CBus(uint8_t sdaPin, uint8_t sclPin) {
  Wire.begin(sdaPin, sclPin);
  Serial.println(F("[I2C] Сканирование..."));
  for (uint8_t address = 1; address < 127; ++address) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print(F("  0x"));
      Serial.println(address, HEX);
    }
  }
}
#endif

bool Aht20Sensor::readHardware() {
#if USE_HARDWARE
  sensors_event_t humidityEvent;
  sensors_event_t tempEvent;
  aht20Chip.getEvent(&humidityEvent, &tempEvent);
  _currentState.temperatureC = tempEvent.temperature;
  _currentState.humidityPct = humidityEvent.relative_humidity;
  _currentState.valid = true;
  return true;
#else
  return false;
#endif
}

bool Aht20Sensor::readSimulated() {
  const unsigned long now = millis();
  if (now - _lastSimUpdateMs >= DATA_UPDATE_INTERVAL_MS) {
    _lastSimUpdateMs = now;
    _currentState.temperatureC += 0.1f;
    _currentState.humidityPct += 0.2f;
  }
  _currentState.valid = true;
  return true;
}
