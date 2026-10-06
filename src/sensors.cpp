/*
sensor communications
SHT31 sensors
SHT31: SCL-> wit, SAA(SDA)->geel
*/
#include "sensors.h"
#include <Wire.h>

Adafruit_SHT31 sht31_inside = Adafruit_SHT31();


bool loadSensorCommunication()
{
    Wire.begin();

    if (!sht31_inside.begin(0x44)) {
        Serial.println("SHT31_inside niet gevonden!");
        return false;
    }
    Serial.println("SHT31_inside gevonden!");
    return true;
}

bool readSensors(DataPoint& data)
{
    data.temperature_inside = sht31_inside.readTemperature();
    data.humidity_inside = sht31_inside.readHumidity();

    if (isnan(data.temperature_inside) || isnan(data.humidity_inside)){
        return false;
    }
    return true;
}
