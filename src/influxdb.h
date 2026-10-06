#pragma once

#include <Arduino.h>
#include "storage.h"
#include "config.h"

String createInfluxLine(const DataPoint& data);
bool postInfluxData(const DataPoint& data);
bool uploadDataPoint(const DataPoint& data);