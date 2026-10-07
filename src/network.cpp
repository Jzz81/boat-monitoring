#include "network.h"
#include "output.h"
#include <WiFi.h>


bool initWifi()
{
    WiFi.mode(WIFI_STA);
    outputInfo("WIFI","Initializing Wifi...");
    return true;
}

void scanWifi()
{
    outputInfo("WIFI","Scanning for WiFi networks...");

    int numberOfNetworks = WiFi.scanNetworks();

    if (numberOfNetworks < 0)
    {
        outputWarning("WIFI","WiFi scan failed!");
        return;
    }

    outputInfo("WIFI","Number of networks: " + String(numberOfNetworks));

    for (int i = 0; i < numberOfNetworks; i++)
    {
        String line;
        line = String(i + 1);
        line += ": ";
        line += WiFi.SSID(i);
        line += "  RSSI: ";
        line += WiFi.RSSI(i);
        line += " dBm";

        if (WiFi.encryptionType(i) == WIFI_AUTH_OPEN)
            line += "  OPEN";
        else
            line += "  secured";
        outputInfo("WIFI",line);
    }

    WiFi.scanDelete();
}

bool connectWifi(const String& ssid, const String& password)
{
    outputInfo("WIFI","Connecting to WiFi: " + ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    uint32_t start = millis();

    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - start > 10000)
        {
            outputWarning("WIFI","WiFi connection failed");
            return false;
        }

        delay(250);
        outputSerial(".");
    }

    outputSerial("\n");
    outputInfo("WIFI","WiFi connected");
    outputInfo("WIFI","IP-address: ");
    outputInfo("WIFI",String(WiFi.localIP()));

    return true;
}

bool disconnectWifi()
{
    outputInfo("WIFI","Disconnecting WiFi...");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    return true;
}

bool checkWifiConnected()
{
    outputSerial("Checking wifi connection...");
    if (WiFi.status() == WL_CONNECTED)
    {
        outputSerial(" connected.\n");
        return true;
    }

    outputSerial(" not connected.\n");
    WiFi.disconnect();
    return false;

}