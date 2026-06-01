#pragma once

#include <Arduino.h>
#include "config.h"

// Циклический буфер из 30 значений: каждую секунду новое, самое старое «забывается».
class CyclicDataBuffer {
public:
  CyclicDataBuffer();

  void begin();
  void update();

  float getCurrentValue() const;
  float getPreviousValue() const;
  size_t getCount() const;

  float getValueAt(size_t indexFromOldest) const;
  String toJsonArray() const;

private:
  float _buffer[DATA_BUFFER_SIZE];

  size_t _writeIndex;
  size_t _count;

  float _currentValue;
  float _previousValue;

  unsigned long _lastUpdateMs;

  float generateNextValue() const;
  void push(float value);
};
