#include "influxdb.h"
#include "config.h"

#include <WiFi.h>
#include <HTTPClient.h>

#include "output.h"

constexpr size_t MAX_UPLOAD_BATCH = 24;

void uploadDataPoints()
{
    size_t positions[MAX_UPLOAD_BATCH];
    DataPoint dataPoints[MAX_UPLOAD_BATCH];

    size_t position;

    outputInfo("INFLUXDB","looking for upload candidates...");
    if (findUploadDatapointFilePosition(position) != 0)
    {
        outputInfo("INFLUXDB","No upload candidates found.");
        return;
    }

    int count = 0;
    while (count < MAX_UPLOAD_BATCH)
    {
        if (!readDataPoint(position, dataPoints[count]))
            break;
        positions[count] = position;
        count++;
        position += DATA_POINT_SIZE;
    }
    outputInfo("INFLUXDB","Found " + String(count) + "upload candidates.");

    if (count == 0)
        return;

    bool success = uploadInfluxData(createInfluxMultiLine(dataPoints, count));

    if (!success)
        return;

    // PAS NU markeren als uploaded
    for (int i = 0; i < count; i++)
    {
        markAsUploaded(positions[i]);
    }
}

bool uploadInfluxData(const String dataString)
{
HTTPClient http;

    String url = config.influxServer + "/api/v2/write?org=" +
                 config.influxOrg +
                 "&bucket=" +
                 config.influxBucket +
                 "&precision=s";
    
    outputInfo("INFLUXDB","InfluxDB upload:" + dataString);

    http.begin(url);

    http.addHeader("Authorization", "Token " + config.influxToken);
    http.addHeader("Content-Type", "text/plain; charset=utf-8");
    http.addHeader("CF-Access-Client-Id",config.cloudflareClientId);
    http.addHeader("CF-Access-Client-Secret",config.cloudflareClientSecret);

    int httpCode = http.POST(dataString);

    outputInfo("INFLUXDB","InfluxDB HTTP status: " + httpCode);

    http.end();

    return httpCode >= 200 && httpCode < 300;
}

String createInfluxMultiLine(const DataPoint dataPoints[MAX_UPLOAD_BATCH], 
                            const int count)
{
    String lines;
    for (int i = 0; i < count; i++)
    {
        lines += createInfluxLine(dataPoints[i]);
        lines += "\n";
    }
    return lines;
}

String createInfluxLine(const DataPoint& data)
{
    String line;

    line += "boat";
    line += ",device=";
    line += config.deviceName;

    line += " temperature_inside=";
    line += String(data.temperature_inside, 2);

    line += ",humidity_inside=";
    line += String(data.humidity_inside, 2);

    line += ",temperature_outside=";
    line += String(data.temperature_outside, 2);

    line += ",humidity_outside=";
    line += String(data.humidity_outside, 2);

    line += ",battery_voltage=";
    line += String(data.battery_voltage, 2);

    line += ",battery_soc=";
    line += String(data.battery_soc, 2);

    line += " ";
    line += String(data.timestamp);

    return line;
}