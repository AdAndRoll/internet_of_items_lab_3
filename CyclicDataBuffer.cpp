#include "CyclicDataBuffer.h"
#include <math.h>

CyclicDataBuffer::CyclicDataBuffer()
    : _buffer{0},
      _writeIndex(0),
      _count(0),
      _currentValue(0.0f),
      _previousValue(0.0f),
      _lastUpdateMs(0) {}

void CyclicDataBuffer::begin() {
  _lastUpdateMs = millis();
  push(generateNextValue());
}

void CyclicDataBuffer::update() {
  const unsigned long now = millis();
  if (now - _lastUpdateMs < DATA_UPDATE_INTERVAL_MS) {
    return;
  }

  _lastUpdateMs = now;
  push(generateNextValue());
}

float CyclicDataBuffer::getCurrentValue() const {
  return _currentValue;
}

float CyclicDataBuffer::getPreviousValue() const {
  return _previousValue;
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

  const size_t startIndex = _writeIndex;
  const size_t physicalIndex = (startIndex + indexFromOldest) % DATA_BUFFER_SIZE;
  return _buffer[physicalIndex];
}

String CyclicDataBuffer::toJsonArray() const {
  String json = F("[");
  for (size_t i = 0; i < _count; ++i) {
    if (i > 0) {
      json += ',';
    }
    json += String(getValueAt(i), 2);
  }
  json += ']';
  return json;
}

float CyclicDataBuffer::generateNextValue() const {
  const float phase = millis() / 10000.0f;
  return 20.0f + 10.0f * sinf(phase) + static_cast<float>(random(-50, 51)) / 10.0f;
}

void CyclicDataBuffer::push(float value) {
  _previousValue = _currentValue;
  _currentValue = value;

  _buffer[_writeIndex] = value;
  _writeIndex = (_writeIndex + 1) % DATA_BUFFER_SIZE;

  if (_count < DATA_BUFFER_SIZE) {
    ++_count;
  }
}
