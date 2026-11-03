#pragma once

#include <Arduino.h>
#include "config.h"

class CyclicDataBuffer {
public:
  CyclicDataBuffer();

  void begin();
  void pushSample(float value, uint32_t timestampSec);

  float getCurrentValue() const;
  size_t getCount() const;
  float getValueAt(size_t indexFromOldest) const;
  uint32_t getTimestampAt(size_t indexFromOldest) const;

  String toJsonArray() const;
  String toTimestampsJsonArray() const;

private:
  float _buffer[DATA_BUFFER_SIZE];
  uint32_t _timestamps[DATA_BUFFER_SIZE];

  size_t _writeIndex;
  size_t _count;
  float _currentValue;
  float _previousValue;

  void push(float value, uint32_t timestampSec);
};
