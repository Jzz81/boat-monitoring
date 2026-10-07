#include <Arduino.h>
#include "config.h"
#include "storage.h"
#include "sensors.h"
#include "clock.h"
#include "network.h"
#include "influxdb.h"
#include "sleep.h"
#include "webserver.h"
#include "output.h"

#include <LittleFS.h>

uint32_t id;
constexpr uint32_t WIFI_CHECK_INTERVAL = 5000;
uint32_t lastWifiCheck = millis() - WIFI_CHECK_INTERVAL;

void measurement() {
    DataPoint data = DataPoint();
    data.id = id++;

    if (!readSensors(data)){
        outputWarning("MAIN","Sensor error!");
    }
    readTime(data);
    setCurrentData(data);

    //TO BE REFACTORED:
    //logica scheiden tussen online en offline in 2 routines.

    if (storeDataPoint(data) == StorageResult::FLUSHED) {
        if (config.mode == MODE_OFFLINE)
        {
            if (!connectWifi(config.offlineWifiSsid,
                        config.offlineWifiPassword))
            {
                outputWarning("MAIN","Could not connect to wifi.");
            } else {
                if(syncTime())
                {
                    uploadDataPoints();
                } else {
                    outputWarning("MAIN","Could not synchronize time. Upload skipped.");
                }
                disconnectWifi();
            }
        } else {
            if(syncTime())
            {
                uploadDataPoints();
            } else {
                outputWarning("MAIN","Could not synchronize time. Upload skipped.");
            }
        }
    }
    printDataPoint(data);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    if (initStorage()){
        outputInfo("MAIN","LittleFS OK");
    } else {
        outputWarning("MAIN","LittleFS FAILED");
    }

    if (!loadConfig())
    {
        outputWarning("MAIN","Could not load config, reverting to default config...");
        loadDefaultConfig();
    }
    outputInfo("MAIN","config loaded.");
    printConfig();

    if (!loadSensorCommunication()){
        outputWarning("MAIN","sensor communication failed...");
    }
    //FOR DEBUG ONLY:
    config.offline_measurement_interval_seconds = 60;
    //clearMeasurements(); // clear measurements file

    if (!lastStoredId(id))
    {
        id = 0;
    }

    if (config.mode == MODE_ONLINE)
    {
        outputInfo("MAIN","Starting ONLINE mode...");

        connectWifi(config.onlineWifiSsid, config.onlineWifiPassword);
        initWebServer();
    } else {

        outputInfo("MAIN","Starting OFFLINE mode...");
        if (!wokeFromTimer())
        {
            //connect to wifi and sync time value.
            connectWifi(config.offlineWifiSsid, config.offlineWifiPassword);
            syncTime();
            disconnectWifi();
        }
        measurement();
        deepSleepSeconds(config.offline_measurement_interval_seconds);
    }
}

uint32_t lastMeasurement = 0;

void loop() {
    if (config.mode == MODE_ONLINE)
    {
        if (millis() - lastWifiCheck >= WIFI_CHECK_INTERVAL)
        {
            if(!checkWifiConnected())
            {
                connectWifi(config.onlineWifiSsid, config.onlineWifiPassword);
            }
            lastWifiCheck = millis();
        }

        handleWebServer();
        if (lastMeasurement == 0 || 
            millis()-lastMeasurement >= config.online_measurement_interval_seconds * 1000UL)
        {
            measurement();
            lastMeasurement = millis();
       }

    }
}
