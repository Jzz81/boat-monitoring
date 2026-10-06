#include "network.h"
#include <WiFi.h>

bool initWifi()
{
    WiFi.mode(WIFI_STA);
    Serial.println("Wifi intiatialiseren...");
    return true;
}

void scanWifi()
{
    Serial.println("WiFi netwerken zoeken...");

    int numberOfNetworks = WiFi.scanNetworks();

    if (numberOfNetworks < 0)
    {
        Serial.println("WiFi scan mislukt!");
        return;
    }

    Serial.print("Aantal netwerken: ");
    Serial.println(numberOfNetworks);

    for (int i = 0; i < numberOfNetworks; i++)
    {
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(WiFi.SSID(i));
        Serial.print("  RSSI: ");
        Serial.print(WiFi.RSSI(i));
        Serial.print(" dBm");

        if (WiFi.encryptionType(i) == WIFI_AUTH_OPEN)
            Serial.println("  OPEN");
        else
            Serial.println("  beveiligd");
    }

    WiFi.scanDelete();
}

bool connectWifi(const String& ssid, const String& password)
{
    Serial.print("Verbinden met WiFi: ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    uint32_t start = millis();

    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - start > 10000)
        {
            Serial.println("WiFi verbinding mislukt");
            return false;
        }

        delay(250);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi verbonden");
    Serial.print("IP-adres: ");
    Serial.println(WiFi.localIP());

    return true;
}

bool disconnectWifi()
{
    Serial.println("WiFi verbinding verbreken...");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    return true;
}