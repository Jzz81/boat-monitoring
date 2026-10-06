#pragma once

#include <Arduino.h>
#include "settings.h"

enum SystemMode {
    MODE_OFFLINE = 0,
    MODE_ONLINE = 1
};

struct Config {
    SystemMode mode;

    uint16_t measurement_interval_seconds;

    String deviceName;

    String offlineWifiSsid;
    String offlineWifiPassword;

    String onlineWifiSsid;
    String onlineWifiPassword;

    String influxServer;
    String influxOrg;
    String influxBucket;
    String influxToken;
};

extern Config config;

bool loadConfig();
void loadDefaultConfig();
bool saveConfig();
void printConfig();

extern Config config;