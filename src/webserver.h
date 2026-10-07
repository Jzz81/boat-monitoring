#pragma once

#include "storage.h"

void initWebServer();
void handleRoot();
void handleWebServer();
void setCurrentData(const DataPoint& data);
void handleData();
void handleSettings();
void handleSaveSettings();