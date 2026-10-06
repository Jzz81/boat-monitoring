#include "influxdb.h"
#include "config.h"

#include <WiFi.h>
#include <HTTPClient.h>

bool postInfluxData(const DataPoint& data)
{
    /*
    "http://localhost:8086/api/v2/write?org=Orion&bucket=measurements&precision=s" \
  --header "Authorization: Token $INFLUX_TOKEN" \
  --header "Content-Type: text/plain; charset=utf-8" \
  --data-binary "boat,device=esp32-test temperature_inside=18.7,humidity_inside=71.0,battery_voltage=12.61,battery_soc=82.0 $(date -d '2 hours ago' +%s)"

    */
   return true;
}

bool uploadDataPoint(const DataPoint& data)
{
HTTPClient http;

    String url = config.influxServer + "/api/v2/write?org=" +
                 config.influxOrg +
                 "&bucket=" +
                 config.influxBucket +
                 "&precision=s";

    String line = createInfluxLine(data);

    Serial.println("InfluxDB upload:");
    Serial.println(line);

    http.begin(url);

    http.addHeader("Authorization", "Token " + config.influxToken);
    http.addHeader("Content-Type", "text/plain; charset=utf-8");

    int httpCode = http.POST(line);

    Serial.print("InfluxDB HTTP status: ");
    Serial.println(httpCode);

    http.end();

    return httpCode >= 200 && httpCode < 300;
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