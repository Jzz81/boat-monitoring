#include "clock.h"
#include <sys/time.h>
#include <time.h>   // nodig voor configTime()

RTC_DATA_ATTR uint32_t lastTimeSync = 0;

bool syncTime()
{
     Serial.println("Synchronizing time with NTP...");
    //Use only UTC:
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    Serial.println("Waiting for NTP time...");

    struct tm timeinfo;

    for (int i = 0; i < 20; i++)
    {
        if (getLocalTime(&timeinfo, 1000))
        {
            lastTimeSync = getTimestamp();

            Serial.println("Time synchronized.");
            Serial.print("Unix time: ");
            Serial.println(lastTimeSync);
            return true;
        }

        Serial.print(".");
    }

    Serial.println();
    Serial.println("NTP synchronization failed.");

    return false;
}

uint32_t getUnixTime()
{
    time_t now;
    time(&now);

    return (uint32_t)now;
}

bool isTimeValid()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    
    // Unix timestamps voor 2020+ zijn groter dan dit
    return tv.tv_sec > 1577836800;
}

uint32_t getTimestamp()
{
    time_t now;
    time(&now);
    return (uint32_t)now;
}

void readTime(DataPoint& data)
{
    data.timestamp = getTimestamp();
    data.time_valid = isTimeValid();
    data.seconds_since_sync = data.timestamp - lastTimeSync;
}
