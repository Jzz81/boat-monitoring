#include "output.h"

#include <LittleFS.h>
#include "clock.h"

namespace
{
    const char* LOG_FILE = "/log.txt";

    void writeOutput(const char* level,
                     const char* source,
                     const String& message)
    {
        uint32_t timestamp = getTimestamp();

        // Naar Serial
        Serial.print(timestamp);
        Serial.print(" | ");
        Serial.print(level);
        Serial.print(" | ");
        Serial.print(source);
        Serial.print(" | ");
        Serial.println(message);

        // Naar logbestand
        File file = LittleFS.open(LOG_FILE, FILE_APPEND);

        if (!file)
        {
            Serial.println("ERROR | OUTPUT | Could not open log.txt");
            return;
        }

        file.print(timestamp);
        file.print("|");
        file.print(level);
        file.print("|");
        file.print(source);
        file.print("|");
        file.println(message);

        file.close();
    }
}

void outputInfo(const char* source, const String& message)
{
    writeOutput("INFO", source, message);
}

void outputWarning(const char* source, const String& message)
{
    writeOutput("WARNING", source, message);
}

void outputError(const char* source, const String& message)
{
    writeOutput("ERROR", source, message);
}

void outputSerial(const String& value)
{
    Serial.print(value);
}

void outputSerial(const char* value)
{
    Serial.print(value);
}

void outputSerial(int value)
{
    Serial.print(value);
}

void outputSerial(unsigned int value)
{
    Serial.print(value);
}

void outputSerial(long value)
{
    Serial.print(value);
}

void outputSerial(unsigned long value)
{
    Serial.print(value);
}

void outputSerial(float value)
{
    Serial.print(value);
}

void outputSerial(double value)
{
    Serial.print(value);
}

void outputSerial(bool value)
{
    Serial.print(value ? "true" : "false");
}