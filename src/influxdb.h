#pragma once

#include <Arduino.h>
#include "storage.h"
#include "config.h"

void uploadDataPoints();
bool uploadInfluxData(const String dataString);
String createInfluxLine(const DataPoint& data);
String createInfluxMultiLine(const DataPoint dataPoints[24], 
                            const int count);
