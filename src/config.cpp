#include "config.h"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "output.h"

Config config;

bool loadConfig()
{
    outputInfo("CONFIG","Loading config...");

    File file = LittleFS.open("/settings.json", FILE_READ);

    if (!file)
    {
        outputWarning("CONFIG","Kan settings.json niet openen");
        return false;
    }
    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, file);

    file.close();

    if (error)
    {
        outputWarning("CONFIG","Fout bij lezen settings.json: " + String(error.c_str()));
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

    config.cloudflareClientId =
        doc["cloudflare_client_id"].as<String>();
    config.cloudflareClientSecret =
        doc["cloudflare_client_secret"].as<String>();

        config.offline_measurement_interval_seconds = doc["offline_measurements_interval_seconds"] | 600;
    config.online_measurement_interval_seconds = doc["online_measurements_interval_seconds"] | 600;
    config.mode = MODE_ONLINE;

    return true;
}

void loadDefaultConfig()
{
    outputInfo("CONFIG","Loading default config.");
    config.deviceName = "boat-monitor";

    config.offlineWifiSsid = "";
    config.offlineWifiPassword = "";

    config.onlineWifiSsid = "";
    config.onlineWifiPassword = "";

    config.influxServer = "";
    config.influxOrg = "";
    config.influxBucket = "";
    config.influxToken = "";

    config.cloudflareClientId = "";
    config.cloudflareClientSecret = "";

    config.offline_measurement_interval_seconds = 600;
    config.online_measurement_interval_seconds = 2;
    config.mode = MODE_ONLINE;
}

bool saveConfig()
{
    
    outputInfo("CONFIG","Saving config...");

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

    doc["cloudflare_client_id"] = config.cloudflareClientId;
    doc["cloudflare_client_secret"] = config.cloudflareClientSecret;

    doc["offline_measurement_interval_seconds"] =
        config.offline_measurement_interval_seconds;
    doc["online_measurement_interval_seconds"] =
        config.online_measurement_interval_seconds;

    File file = LittleFS.open("/settings.json", FILE_WRITE);

    if (!file)
    {
        outputWarning("CONFIG","Kan settings.json niet openen voor schrijven");
        return false;
    }

    if (serializeJsonPretty(doc, file) == 0)
    {
        outputWarning("CONFIG","Fout bij schrijven settings.json");
        file.close();
        return false;
    }

    file.close();

    outputInfo("CONFIG","Config opgeslagen");
    return true;
}

void printConfig()
{
    outputSerial("\n");
    outputSerial("===== CONFIG =====\n");

    outputSerial("Mode: ");
    outputSerial((int)config.mode);
    outputSerial("\n");

    outputSerial("Online measurement interval: ");
    outputSerial(config.online_measurement_interval_seconds);
    outputSerial(" seconds\n");

    outputSerial("Offline measurement interval: ");
    outputSerial(config.offline_measurement_interval_seconds);
    outputSerial(" seconds\n");

    outputSerial("Device name: ");
    outputSerial(config.deviceName);
    outputSerial("\n");

    outputSerial("\n--- Offline WiFi ---\n");

    outputSerial("SSID: ");
    outputSerial(config.offlineWifiSsid);
    outputSerial("\n");

    outputSerial("Password: ");
    outputSerial(config.offlineWifiPassword.isEmpty() ? "<empty>" : "<set>\n");

    outputSerial("\n--- Online WiFi ---\n");

    outputSerial("SSID: ");
    outputSerial(config.onlineWifiSsid);
    outputSerial("\n");

    outputSerial("Password: ");
    outputSerial(config.onlineWifiPassword.isEmpty() ? "<empty>" : "<set>\n");

    outputSerial("\n--- InfluxDB ---\n");

    outputSerial("Server: ");
    outputSerial(config.influxServer);
    outputSerial("\n");

    outputSerial("Organization: ");
    outputSerial(config.influxOrg);
    outputSerial("\n");

    outputSerial("Bucket: ");
    outputSerial(config.influxBucket);
    outputSerial("\n");

    outputSerial("Token: ");
    outputSerial(config.influxToken.isEmpty() ? "<empty>" : "<set>\n");

    outputSerial("==================\n");
    outputSerial("\n");
}
