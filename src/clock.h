#pragma once

#include <Arduino.h>
#include "storage.h"

constexpr uint32_t TIME_VALID_PERIOD = 86400;

bool syncTime();
uint32_t getUnixTime();
bool isTimeValid();
uint32_t getTimestamp();
void readTime(DataPoint& data);
