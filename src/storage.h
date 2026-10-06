#pragma once

#include <Arduino.h>

#define FILE_READ_WRITE "r+"

struct DataPoint {
    uint32_t id;

    uint32_t timestamp;
    uint32_t seconds_since_sync;//time since last time sync. For validating timestamps.
    bool time_valid; //to flag if a time is a valid time (synced to network time)

    float temperature_inside;
    float humidity_inside;

    float temperature_outside;
    float humidity_outside;

    float battery_voltage;
    float battery_soc;

    bool uploaded; //to flag if this data is uploaded
};

constexpr size_t DATA_POINT_SIZE = sizeof(DataPoint);

//6 metingen opslaan in RTC
constexpr size_t RTC_BUFFER_SIZE = 6;

extern RTC_DATA_ATTR DataPoint rtcBuffer[RTC_BUFFER_SIZE];
extern RTC_DATA_ATTR int rtcCount;

enum class StorageResult {
    BUFFERED,
    FLUSHED,
    ERROR
};

bool initStorage(bool format = false);

bool lastStoredId(uint32_t& id);

StorageResult storeDataPoint(const DataPoint& data);

bool writeRTCBufferToFlash();

bool writeDataToFlash(const DataPoint& data);

int findUploadDatapointFilePosition(size_t& position);
int findNonValidTimeDataPointFilePosition(size_t& position);

bool readDataPoint(size_t& position, DataPoint& data);

bool markAsUploaded(size_t& position);

bool purgeOldData();

void printDataPoint(const DataPoint& data);

void clearMeasurements();