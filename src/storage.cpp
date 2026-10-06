/*
code for storage of data
- short term storage in RTC
- longer storage in flash mem (with LittleFS)
*/
#include "storage.h"
#include <LittleFS.h>

RTC_DATA_ATTR DataPoint rtcBuffer[6];
RTC_DATA_ATTR int rtcCount = 0;

bool initStorage(bool format)
{
    Serial.println("Starting LittleFS...");

    if (format)
    {
        return LittleFS.begin(true);
    }
    return LittleFS.begin(false, "/littlefs", 10, "spiffs");
}

bool lastStoredId(uint32_t& id)
{
    //find the last stored ID in Flash mem
    File file = LittleFS.open("/measurements.bin", FILE_READ);
    if (!file) 
    {
        Serial.println("File '/measurements.bin' could not be read.");
        return false;
    }
    size_t lastPosition = file.size() - DATA_POINT_SIZE;
    
    if (!file.seek(lastPosition))
    {
        file.close();
        return false;
    }
    DataPoint data;
    
    if (file.read((uint8_t*)&data, DATA_POINT_SIZE) != DATA_POINT_SIZE)
    {
        file.close();
        return false;
    }
    id = data.id;
    Serial.print("Found last data point in flash, id is: ");
    Serial.println(id);

    if (rtcCount > 0)
    {
        //there are datapoints in the RTC memory
        if (rtcBuffer[rtcCount-1].id > id)
        {
            //id value is higher, this is a valid id.
            id = rtcBuffer[rtcCount-1].id;
        }
    }
    
    file.close();
    return true;
}

StorageResult storeDataPoint(const DataPoint& data)
{
    if (rtcCount == 6){
        Serial.println("buffer full, writing to flash");
        writeRTCBufferToFlash();
        rtcCount = 0;
        rtcBuffer[rtcCount] = data;
        rtcCount++;
        return StorageResult::FLUSHED;
    }

    rtcBuffer[rtcCount] = data;
    rtcCount++;
    return StorageResult::BUFFERED;
}

bool writeRTCBufferToFlash()
{
    //write all Data points in RTC to flash memory
    for (int i = 0; i < 6; i++){
        Serial.print("writing data with id: ");
        Serial.print(rtcBuffer[i].id);
        Serial.println(" to flash.");
        writeDataToFlash(rtcBuffer[i]);
    }
    return true;
}

bool writeDataToFlash(const DataPoint& data)
{
    File file = LittleFS.open("/measurements.bin", FILE_APPEND);

    if (!file){
        Serial.println("Kan measurements.bin niet openen");
        return false;
    }
    size_t written = file.write(
        (const uint8_t*)&data,
        DATA_POINT_SIZE
    );

    file.close();

    return written == DATA_POINT_SIZE;
    return true;
}

int findNonValidTimeDataPointFilePosition(size_t& position)
{
    //wil find the first DataPoint in the 'measurements.bin' file where time is not valid
    //return 0 if one is found (position stored in reference 'position')
    //return -1 on error
    //return -2 if no non-valid time values are present.
    File file = LittleFS.open("/measurements.bin", FILE_READ);
    if (!file) 
    {
        return -1;
    }

    size_t fileSize = file.size();
    position = file.size();

    while(fileSize >= DATA_POINT_SIZE)
    {
        fileSize -= DATA_POINT_SIZE;
        
        if (!file.seek(fileSize))
        {
            file.close();
            return -1;
        }

        DataPoint data;
        
        if (file.read((uint8_t*)&data, DATA_POINT_SIZE) != DATA_POINT_SIZE)
        {
            file.close();
            return -1;
        }

        if (data.time_valid)
        {
            if (position == file.size())
            {
                file.close();
                return -2;
            }
            file.close();
            return 0;
        } else {
            position = fileSize;
        }
    }
    // The whole file consisted of non-uploaded datapoints.
    if (position != file.size())
    {
        file.close();
        return 0;
    }

    file.close();
    return -2;

}

int findUploadDatapointFilePosition(size_t& position)
{
    //wil find the first upload candidate in the 'measurements.bin' file.
    //return 0 if one is found (position stored in reference 'position')
    //return -1 on error
    //return -2 if no upload candidates are present.
    File file = LittleFS.open("/measurements.bin", FILE_READ);
    if (!file) 
    {
        return -1;
    }

    size_t fileSize = file.size();
    position = file.size();

    while(fileSize >= DATA_POINT_SIZE)
    {
        fileSize -= DATA_POINT_SIZE;
        
        if (!file.seek(fileSize))
        {
            file.close();
            return -1;
        }

        DataPoint data;
        
        if (file.read((uint8_t*)&data, DATA_POINT_SIZE) != DATA_POINT_SIZE)
        {
            file.close();
            return -1;
        }

        if (data.uploaded)
        {
            if (position == file.size())
            {
                file.close();
                return -2;
            }
            file.close();
            return 0;
        } else {
            position = fileSize;
        }
    }
    // The whole file consisted of non-uploaded datapoints.
    if (position != file.size())
    {
        file.close();
        return 0;
    }

    file.close();
    return -2;
}

bool readDataPoint(size_t& position, DataPoint& data)
{
    //read a datapoint from the 'measurements.bin' file
    File file = LittleFS.open("/measurements.bin", FILE_READ);
    if (!file) 
    {
        return false;
    }

    if (!file.seek(position))
    {
        file.close();
        return false;
    }
    
    if (file.read((uint8_t*)&data, DATA_POINT_SIZE) != DATA_POINT_SIZE)
    {
        file.close();
        return false;
    }

    return true;
}

bool markAsUploaded(size_t& position)
{
    File file = LittleFS.open("/measurements.bin", FILE_READ_WRITE);
    if (!file)
        return false;

    if (!file.seek(position))
    {
        file.close();
        return false;
    }
    DataPoint data;

    // DataPoint lezen
    if (file.read((uint8_t*)&data, DATA_POINT_SIZE) != DATA_POINT_SIZE)
    {
        file.close();
        return false;
    }

    data.uploaded = true;

    if (!file.seek(position))
    {
        file.close();
        return false;
    }

    size_t written = file.write(
        (const uint8_t*)&data,
        DATA_POINT_SIZE
    );

    file.close();

    return written == DATA_POINT_SIZE;

}

bool purgeOldData()
{
    // later:
    // oude records verwijderen
    return true;
}

void printDataPoint(const DataPoint& data)
{
    Serial.print("===DATAPOINT ID: ");
    Serial.print(data.id);
    Serial.println(" ===");

    Serial.print("Timestamp: ");
    Serial.println(data.timestamp);

    Serial.print("Valid time: ");
    if (data.time_valid) {
        Serial.println("yes");
    } else {
        Serial.println("no");
    }

    Serial.print("Uptime: ");
    Serial.print(data.seconds_since_sync);
    Serial.println(" seconds");

    Serial.print("Temperature Inside: ");
    Serial.print(data.temperature_inside);
    Serial.println(" °C");

    Serial.print("Humidity Inside: ");
    Serial.print(data.humidity_inside);
    Serial.println(" %");

    Serial.print("Temperature Outside: ");
    Serial.print(data.temperature_outside);
    Serial.println(" °C");

    Serial.print("Humidity Outside: ");
    Serial.print(data.humidity_outside);
    Serial.println(" %");

    Serial.print("Battery voltage: ");
    Serial.print(data.battery_voltage);
    Serial.println(" V");

    Serial.print("Battery SOC: ");
    Serial.print(data.battery_soc);
    Serial.println(" %");

    Serial.println("---");
}

void clearMeasurements()
{
    Serial.println("Clearing measurements.bin...");

    if (LittleFS.remove("/measurements.bin"))
    {
        Serial.println("measurements.bin deleted.");
    }
    else
    {
        Serial.println("Could not delete measurements.bin.");
    }
}