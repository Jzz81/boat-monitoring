#include <Arduino.h>
#include "config.h"
#include "storage.h"
#include "sensors.h"
#include "clock.h"
#include "network.h"
#include "influxdb.h"
#include "sleep.h"

#include <LittleFS.h>

uint32_t id;


void uploadDataPoints()
{
    Serial.println("Find first data to upload...");

    size_t position;
    int result = findUploadDatapointFilePosition(position);
    if(result==-1)
    {
        Serial.println("Could not read file for upload candidates");
        return;
    }

    if (result==-2)
    {
        Serial.println("No upload candidates");
        return;
    }

    while (result == 0)
    {
        //upload candidates found.
        DataPoint data;
        if (readDataPoint(position,data))
        {
            if (uploadDataPoint(data))
            {
                if (!markAsUploaded(position))
                {
                    Serial.println("Could not markt data point as uploaded (upload was succesfull)...");
                }
            }
        }
        result = findUploadDatapointFilePosition(position);
    }
}

void measurement() {
    DataPoint data = DataPoint();
    data.id = id++;

    if (!readSensors(data)){
        Serial.println("Sensor error!");
    }
    readTime(data);
    if (storeDataPoint(data) == StorageResult::FLUSHED) {
        if (!connectWifi(config.offlineWifiSsid,
                    config.offlineWifiPassword))
        {
            Serial.println("Could not connect to wifi.");
        } else {
            if(syncTime())
            {
                uploadDataPoints();
            } else {
                Serial.println("Could not synchronize time. Upload skipped.");
            }
            disconnectWifi();
        }
    }
    printDataPoint(data);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    if (initStorage()){
        Serial.println("LittleFS OK");
    } else {
        Serial.println("LittleFS FAILED");
    }

    if (!loadConfig())
    {
        Serial.println("Could not load config, reverting to default config...");
        loadDefaultConfig();
    }
    Serial.println("config loaded.");
    printConfig();

    if (!loadSensorCommunication()){
        Serial.println("sensor communication failed...");
    }

    //FOR DEBUG ONLY:
    config.measurement_interval_seconds = 60;
    //clearMeasurements(); // clear measurements file

    if (!lastStoredId(id))
    {
        id = 0;
    }
    if (!wokeFromTimer())
    {
        //connect to wifi and sync time value.
        connectWifi(config.offlineWifiSsid, config.offlineWifiPassword);
        syncTime();
        disconnectWifi();
    }
    measurement();
    deepSleepSeconds(config.measurement_interval_seconds);
}


void loop() {
    return;
    measurement();

    delay(config.measurement_interval_seconds * 1000);
}
