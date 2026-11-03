#include "CyclicDataBuffer.h"
#include <stdio.h>

CyclicDataBuffer::CyclicDataBuffer()
    : _buffer{0},
      _timestamps{0},
      _writeIndex(0),
      _count(0),
      _currentValue(0.0f),
      _previousValue(0.0f) {}

void CyclicDataBuffer::begin() {
  _writeIndex = 0;
  _count = 0;
  _currentValue = 0.0f;
  _previousValue = 0.0f;
}

void CyclicDataBuffer::pushSample(float value, uint32_t timestampSec) {
  push(value, timestampSec);
}

float CyclicDataBuffer::getCurrentValue() const {
  return _currentValue;
}

size_t CyclicDataBuffer::getCount() const {
  return _count;
}

float CyclicDataBuffer::getValueAt(size_t indexFromOldest) const {
  if (_count == 0 || indexFromOldest >= _count) {
    return 0.0f;
  }
  if (_count < DATA_BUFFER_SIZE) {
    return _buffer[indexFromOldest];
  }
  return _buffer[(_writeIndex + indexFromOldest) % DATA_BUFFER_SIZE];
}

uint32_t CyclicDataBuffer::getTimestampAt(size_t indexFromOldest) const {
  if (_count == 0 || indexFromOldest >= _count) {
    return 0;
  }
  if (_count < DATA_BUFFER_SIZE) {
    return _timestamps[indexFromOldest];
  }
  return _timestamps[(_writeIndex + indexFromOldest) % DATA_BUFFER_SIZE];
}

String CyclicDataBuffer::toJsonArray() const {
  String json = F("[");
  for (size_t i = 0; i < _count; ++i) {
    if (i > 0) {
      json += ',';
    }
    char num[16];
    snprintf(num, sizeof(num), "%.2f", getValueAt(i));
    json += num;
  }
  json += ']';
  return json;
}

String CyclicDataBuffer::toTimestampsJsonArray() const {
  String json = F("[");
  for (size_t i = 0; i < _count; ++i) {
    if (i > 0) {
      json += ',';
    }
    json += getTimestampAt(i);
  }
  json += ']';
  return json;
}

void CyclicDataBuffer::push(float value, uint32_t timestampSec) {
  _previousValue = _currentValue;
  _currentValue = value;

  _buffer[_writeIndex] = value;
  _timestamps[_writeIndex] = timestampSec;
  _writeIndex = (_writeIndex + 1) % DATA_BUFFER_SIZE;

  if (_count < DATA_BUFFER_SIZE) {
    ++_count;
  }
}
