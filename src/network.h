#pragma once

#include <Arduino.h>

bool initWifi();
void scanWifi();
bool connectWifi(const String& ssid, const String& password);
bool disconnectWifi();

