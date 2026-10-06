#pragma once

#include <Arduino.h>
#include <Adafruit_SHT31.h>
#include "storage.h"

bool loadSensorCommunication();
bool readSensors(DataPoint& data);