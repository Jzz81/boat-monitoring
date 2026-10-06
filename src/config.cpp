#include "config.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

Config config;

bool loadConfig()
{
    Serial.println("Loading config...");

    File file = LittleFS.open("/settings.json", FILE_READ);

    if (!file)
    {
        Serial.println("Kan settings.json niet openen");
        return false;
    }
    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, file);

    file.close();

    if (error)
    {
        Serial.print("Fout bij lezen settings.json: ");
        Serial.println(error.c_str());
        return false;
    }

    config.deviceName = doc["device_name"] | "";

    config.offlineWifiSsid = doc["offline_wifi_ssid"] | "";
    config.offlineWifiPassword = doc["offline_wifi_password"] | "";

    config.onlineWifiSsid = doc["online_wifi_ssid"] | "";
    config.onlineWifiPassword = doc["online_wifi_password"] | "";

    config.influxServer = doc["influx_server"] | "";
    config.influxOrg = doc["influx_org"] | "";
    config.influxBucket = doc["influx_bucket"] | "";
    config.influxToken = doc["influx_token"] | "";

    config.measurement_interval_seconds = doc["measurement_interval_seconds"] | 600;
    config.mode = MODE_OFFLINE;

    return true;
}

void loadDefaultConfig()
{
    config.deviceName = "boat-monitor";

    config.offlineWifiSsid = "";
    config.offlineWifiPassword = "";

    config.onlineWifiSsid = "";
    config.onlineWifiPassword = "";

    config.influxServer = "";
    config.influxOrg = "";
    config.influxBucket = "";
    config.influxToken = "";

    config.measurement_interval_seconds = 600;
    config.mode = MODE_OFFLINE;
}

bool saveConfig()
{
    Serial.println("Saving config...");

    JsonDocument doc;

    doc["device_name"] = config.deviceName;
    doc["offline_wifi_ssid"] = config.offlineWifiSsid;
    doc["offline_wifi_password"] = config.offlineWifiPassword;

    doc["online_wifi_ssid"] = config.onlineWifiSsid;
    doc["online_wifi_password"] = config.onlineWifiPassword;

    doc["influx_server"] = config.influxServer;
    doc["influx_org"] = config.influxOrg;
    doc["influx_bucket"] = config.influxBucket;
    doc["influx_token"] = config.influxToken;

    doc["measurement_interval_seconds"] =
        config.measurement_interval_seconds;

    File file = LittleFS.open("/settings.json", FILE_WRITE);

    if (!file)
    {
        Serial.println("Kan settings.json niet openen voor schrijven");
        return false;
    }

    if (serializeJsonPretty(doc, file) == 0)
    {
        Serial.println("Fout bij schrijven settings.json");
        file.close();
        return false;
    }

    file.close();

    Serial.println("Config opgeslagen");
    return true;
}

void printConfig()
{
    Serial.println();
    Serial.println("===== CONFIG =====");

    Serial.print("Mode: ");
    Serial.println((int)config.mode);

    Serial.print("Measurement interval: ");
    Serial.print(config.measurement_interval_seconds);
    Serial.println(" seconds");

    Serial.print("Upload interval: ");
    Serial.print(config.measurement_interval_seconds * 6);
    Serial.println(" seconds");

    Serial.print("Device name: ");
    Serial.println(config.deviceName);

    Serial.println();
    Serial.println("--- Offline WiFi ---");

    Serial.print("SSID: ");
    Serial.println(config.offlineWifiSsid);

    Serial.print("Password: ");
    Serial.println(config.offlineWifiPassword.isEmpty() ? "<empty>" : "<set>");

    Serial.println();
    Serial.println("--- Online WiFi ---");

    Serial.print("SSID: ");
    Serial.println(config.onlineWifiSsid);

    Serial.print("Password: ");
    Serial.println(config.onlineWifiPassword.isEmpty() ? "<empty>" : "<set>");

    Serial.println();
    Serial.println("--- InfluxDB ---");

    Serial.print("Server: ");
    Serial.println(config.influxServer);

    Serial.print("Organization: ");
    Serial.println(config.influxOrg);

    Serial.print("Bucket: ");
    Serial.println(config.influxBucket);

    Serial.print("Token: ");
    Serial.println(config.influxToken.isEmpty() ? "<empty>" : "<set>");

    Serial.println("==================");
    Serial.println();
}
