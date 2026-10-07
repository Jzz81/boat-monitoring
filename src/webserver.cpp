/*
simple webserver for config, status and debug.
*/
#include "webserver.h"
#include "config.h"
#include <Arduino.h>
#include <WebServer.h>
#include <esp_system.h>
#include <LittleFS.h>

WebServer server(80);
DataPoint currentData;
bool currentDataValid = false;

void handleRoot()
{
    String html;

    html += "<!DOCTYPE html>";
    html += "<html lang='nl'>";

    html += "<head>";
    html += "<meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>Boat Monitor</title>";

    html += "<style>";
    html += "body {";
    html += "  font-family: Arial, sans-serif;";
    html += "  background: #f4f6f8;";
    html += "  color: #222;";
    html += "  margin: 0;";
    html += "  padding: 20px;";
    html += "}";

    html += ".container {";
    html += "  max-width: 700px;";
    html += "  margin: auto;";
    html += "}";

    html += "h1 {";
    html += "  margin-bottom: 25px;";
    html += "}";

    html += ".card {";
    html += "  background: white;";
    html += "  padding: 20px;";
    html += "  margin-bottom: 20px;";
    html += "  border-radius: 10px;";
    html += "  box-shadow: 0 2px 8px rgba(0,0,0,0.08);";
    html += "}";

    html += ".card h2 {";
    html += "  margin-top: 0;";
    html += "  font-size: 20px;";
    html += "}";

    html += ".row {";
    html += "  display: flex;";
    html += "  justify-content: space-between;";
    html += "  gap: 20px;";
    html += "  padding: 8px 0;";
    html += "  border-bottom: 1px solid #eee;";
    html += "}";

    html += ".row:last-child {";
    html += "  border-bottom: none;";
    html += "}";

    html += ".label {";
    html += "  color: #666;";
    html += "}";

    html += ".value {";
    html += "  font-weight: bold;";
    html += "  text-align: right;";
    html += "  word-break: break-word;";
    html += "}";
    
    html += "</style>";
    html += "</head>";

    html += "<script>";
    html += "async function updateData() {";
    html += "  try {";
    html += "    const response = await fetch('/api/data');";
    html += "    if (!response.ok) return;";
    html += "    const data = await response.json();";

    html += "    document.getElementById('temperature_inside').textContent = data.temperature_inside.toFixed(1) + ' °C';";
    html += "    document.getElementById('humidity_inside').textContent = data.humidity_inside.toFixed(1) + ' %';";
    html += "    document.getElementById('temperature_outside').textContent = data.temperature_outside.toFixed(1) + ' °C';";
    html += "    document.getElementById('humidity_outside').textContent = data.humidity_outside.toFixed(1) + ' %';";
    html += "    document.getElementById('battery_voltage').textContent = data.battery_voltage.toFixed(2) + ' V';";
    html += "    document.getElementById('battery_soc').textContent = data.battery_soc.toFixed(1) + ' %';";
    html += "  } catch (error) {";
    html += "    console.log('Geen data beschikbaar');";
    html += "  }";
    html += "}";

    html += "updateData();";
    html += "setInterval(updateData, 2000);";
    html += "</script>";

    html += "<body>";
    html += "<div class='container'>";

    html += "<h1>Boat Monitor</h1>";

    html += "<p><a href='/settings'>⚙ Settings</a></p>";

    html += "<p>Temperatuur binnen: <span id='temperature_inside'>--</span></p>";
    html += "<p>Luchtvochtigheid binnen: <span id='humidity_inside'>--</span></p>";
    // System
    html += "<div class='card'>";
    html += "<h2>Systeem</h2>";

    html += "<div class='row'>";
    html += "<div class='label'>Mode</div>";
    html += "<div class='value'>";
    html += (config.mode == MODE_ONLINE ? "ONLINE" : "OFFLINE");
    html += "</div>";
    html += "</div>";

    html += "<div class='row'>";
    html += "<div class='label'>Device name</div>";
    html += "<div class='value'>";
    html += config.deviceName;
    html += "</div>";
    html += "</div>";

    html += "</div>";

    html += "<div class='card'>";
    html += "<h2>System Status</h2>";

    html += "<div class='row'>";
    html += "<div class='label'>RAM gebruikt</div>";
    html += "<div class='value'>";

    uint32_t totalHeap = ESP.getHeapSize();
    uint32_t freeHeap = ESP.getFreeHeap();
    uint32_t usedHeap = totalHeap - freeHeap;

    html += String(usedHeap / 1024);
    html += " KB / ";
    html += String(totalHeap / 1024);
    html += " KB";

    html += "</div>";
    html += "</div>";
    html += "<div class='row'>";
    html += "<div class='label'>Flash storage gebruikt</div>";
    html += "<div class='value'>";

    size_t totalBytes = LittleFS.totalBytes();
    size_t usedBytes = LittleFS.usedBytes();

    html += String(usedBytes / 1024);
    html += " KB / ";
    html += String(totalBytes / 1024);
    html += " KB";

    if (totalBytes > 0)
    {
        html += " (";
        html += String((usedBytes * 100) / totalBytes);
        html += "%)";
    }

    html += "</div>";
    html += "</div>";
    html += "</div>";


    // WiFi
    html += "<div class='card'>";
    html += "<h2>WiFi</h2>";

    html += "<div class='row'>";
    html += "<div class='label'>Offline SSID</div>";
    html += "<div class='value'>";
    html += config.offlineWifiSsid;
    html += "</div>";
    html += "</div>";

    html += "<div class='row'>";
    html += "<div class='label'>Offline password</div>";
    html += "<div class='value'>********</div>";
    html += "</div>";

    html += "<div class='row'>";
    html += "<div class='label'>Online SSID</div>";
    html += "<div class='value'>";
    html += config.onlineWifiSsid;
    html += "</div>";
    html += "</div>";

    html += "<div class='row'>";
    html += "<div class='label'>Online password</div>";
    html += "<div class='value'>********</div>";
    html += "</div>";

    html += "</div>";

    // InfluxDB
    html += "<div class='card'>";
    html += "<h2>InfluxDB</h2>";

    html += "<div class='row'>";
    html += "<div class='label'>Server</div>";
    html += "<div class='value'>";
    html += config.influxServer;
    html += "</div>";
    html += "</div>";

    html += "<div class='row'>";
    html += "<div class='label'>Organization</div>";
    html += "<div class='value'>";
    html += config.influxOrg;
    html += "</div>";
    html += "</div>";

    html += "<div class='row'>";
    html += "<div class='label'>Bucket</div>";
    html += "<div class='value'>";
    html += config.influxBucket;
    html += "</div>";
    html += "</div>";

    html += "<div class='row'>";
    html += "<div class='label'>Token</div>";
    html += "<div class='value'>********</div>";
    html += "</div>";

    html += "</div>";

    // Cloudflare Access
    html += "<div class='card'>";
    html += "<h2>Cloudflare Access</h2>";

    html += "<div class='row'>";
    html += "<div class='label'>Client ID</div>";
    html += "<div class='value'>";
    html += config.cloudflareClientId.substring(0, 8);
    html += "********";
    html += "</div>";
    html += "</div>";

    html += "<div class='row'>";
    html += "<div class='label'>Client Secret</div>";
    html += "<div class='value'>********</div>";
    html += "</div>";

    html += "</div>";

// Intervals
    html += "<div class='card'>";
    html += "<h2>Meetintervallen</h2>";

    html += "<div class='row'>";
    html += "<div class='label'>Offline</div>";
    html += "<div class='value'>";
    html += String(config.offline_measurement_interval_seconds);
    html += " seconden</div>";
    html += "</div>";

    html += "<div class='row'>";
    html += "<div class='label'>Online</div>";
    html += "<div class='value'>";
    html += String(config.online_measurement_interval_seconds);
    html += " seconden</div>";
    html += "</div>";

    html += "</div>";

    html += "</div>";
    html += "</body>";
    html += "</html>";

    server.send(200, "text/html", html);
}

void initWebServer()
{
    server.on("/", handleRoot);
    server.on("/api/data", handleData);
    server.on("/settings", handleSettings);
    server.on("/save-settings", HTTP_POST, handleSaveSettings);
    server.begin();

    Serial.println("Webserver gestart.");
}

void setCurrentData(const DataPoint& data)
{
    currentData = data;
    currentDataValid = true;
}

void handleWebServer()
{
    server.handleClient();
}

void handleData()
{
    if (!currentDataValid)
    {
        server.send(503, "application/json",
                    "{\"error\":\"No measurement available\"}");
        return;
    }

    String json = "{";

    json += "\"temperature_inside\":";
    json += String(currentData.temperature_inside, 1);

    json += ",\"humidity_inside\":";
    json += String(currentData.humidity_inside, 1);

    json += ",\"temperature_outside\":";
    json += String(currentData.temperature_outside, 1);

    json += ",\"humidity_outside\":";
    json += String(currentData.humidity_outside, 1);

    json += ",\"battery_voltage\":";
    json += String(currentData.battery_voltage, 2);

    json += ",\"battery_soc\":";
    json += String(currentData.battery_soc, 1);

    json += "}";

    server.send(200, "application/json", json);
}

void handleSettings()
{
    String html;

    html += "<!DOCTYPE html>";
    html += "<html lang='nl'>";

    html += "<head>";
    html += "<meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>Boat Monitor - Settings</title>";

    html += "<style>";

    html += "body {";
    html += "  font-family: Arial, sans-serif;";
    html += "  background: #f4f6f8;";
    html += "  color: #222;";
    html += "  margin: 0;";
    html += "  padding: 20px;";
    html += "}";

    html += ".container {";
    html += "  max-width: 700px;";
    html += "  margin: auto;";
    html += "}";

    html += ".card {";
    html += "  background: white;";
    html += "  padding: 20px;";
    html += "  margin-bottom: 20px;";
    html += "  border-radius: 10px;";
    html += "  box-shadow: 0 2px 8px rgba(0,0,0,0.08);";
    html += "}";

    html += ".card h2 {";
    html += "  margin-top: 0;";
    html += "  font-size: 20px;";
    html += "}";

    html += "label {";
    html += "  display: block;";
    html += "  margin-top: 15px;";
    html += "  margin-bottom: 5px;";
    html += "  color: #666;";
    html += "}";

    html += "input {";
    html += "  width: 100%;";
    html += "  box-sizing: border-box;";
    html += "  padding: 10px;";
    html += "  border: 1px solid #ccc;";
    html += "  border-radius: 5px;";
    html += "  font-size: 16px;";
    html += "}";

    html += "button {";
    html += "  margin-top: 20px;";
    html += "  padding: 10px 20px;";
    html += "  border: none;";
    html += "  border-radius: 5px;";
    html += "  background: #1976d2;";
    html += "  color: white;";
    html += "  font-size: 16px;";
    html += "}";

    html += "a {";
    html += "  color: #1976d2;";
    html += "  text-decoration: none;";
    html += "}";

    html += "</style>";
    html += "</head>";

    html += "<body>";
    html += "<div class='container'>";

    html += "<h1>Settings</h1>";

    html += "<p><a href='/'>← Back to dashboard</a></p>";

    html += "<form method='POST' action='/save-settings'>";

    // System
    html += "<div class='card'>";
    html += "<h2>System</h2>";

    html += "<label>Device name</label>";
    html += "<input type='text' name='device_name' value='";
    html += config.deviceName;
    html += "'>";

    html += "</div>";

    // WiFi
    html += "<div class='card'>";
    html += "<h2>WiFi</h2>";

    html += "<label>Offline SSID</label>";
    html += "<input type='text' name='offline_wifi_ssid' value='";
    html += config.offlineWifiSsid;
    html += "'>";

    html += "<label>Offline password</label>";
    html += "<input type='password' name='offline_wifi_password' value='";
    html += config.offlineWifiPassword;
    html += "'>";

    html += "<label>Online SSID</label>";
    html += "<input type='text' name='online_wifi_ssid' value='";
    html += config.onlineWifiSsid;
    html += "'>";

    html += "<label>Online password</label>";
    html += "<input type='password' name='online_wifi_password' value='";
    html += config.onlineWifiPassword;
    html += "'>";

    html += "</div>";

    // InfluxDB
    html += "<div class='card'>";
    html += "<h2>InfluxDB</h2>";

    html += "<label>Server</label>";
    html += "<input type='text' name='influx_server' value='";
    html += config.influxServer;
    html += "'>";

    html += "<label>Organization</label>";
    html += "<input type='text' name='influx_org' value='";
    html += config.influxOrg;
    html += "'>";

    html += "<label>Bucket</label>";
    html += "<input type='text' name='influx_bucket' value='";
    html += config.influxBucket;
    html += "'>";

    html += "<label>Token</label>";
    html += "<input type='password' name='influx_token' value='";
    html += config.influxToken;
    html += "'>";

    html += "</div>";

    // Cloudflare
    html += "<div class='card'>";
    html += "<h2>Cloudflare Access</h2>";

    html += "<label>Client ID</label>";
    html += "<input type='text' name='cloudflare_client_id' value='";
    html += config.cloudflareClientId;
    html += "'>";

    html += "<label>Client Secret</label>";
    html += "<input type='password' name='cloudflare_client_secret' value='";
    html += config.cloudflareClientSecret;
    html += "'>";

    html += "</div>";

    // Intervals
    html += "<div class='card'>";
    html += "<h2>Measurement intervals</h2>";

    html += "<label>Offline interval (seconds)</label>";
    html += "<input type='number' name='offline_measurement_interval_seconds' value='";
    html += String(config.offline_measurement_interval_seconds);
    html += "' min='1'>";

    html += "<label>Online interval (seconds)</label>";
    html += "<input type='number' name='online_measurement_interval_seconds' value='";
    html += String(config.online_measurement_interval_seconds);
    html += "' min='1'>";

    html += "</div>";

    html += "<button type='submit'>Save settings</button>";
    html += "</form>";

    html += "</div>";


    html += "</body>";
    html += "</html>";

    server.send(200, "text/html", html);
}

void handleSaveSettings()
{
    config.deviceName =
        server.arg("device_name");

    config.offlineWifiSsid =
        server.arg("offline_wifi_ssid");

    config.offlineWifiPassword =
        server.arg("offline_wifi_password");

    config.onlineWifiSsid =
        server.arg("online_wifi_ssid");

    config.onlineWifiPassword =
        server.arg("online_wifi_password");

    config.influxServer =
        server.arg("influx_server");

    config.influxOrg =
        server.arg("influx_org");

    config.influxBucket =
        server.arg("influx_bucket");

    config.influxToken =
        server.arg("influx_token");

    config.cloudflareClientId =
        server.arg("cloudflare_client_id");

    config.cloudflareClientSecret =
        server.arg("cloudflare_client_secret");

    config.offline_measurement_interval_seconds =
        server.arg("offline_measurement_interval_seconds").toInt();

    config.online_measurement_interval_seconds =
        server.arg("online_measurement_interval_seconds").toInt();

    if (!saveConfig())
    {
        server.send(
            500,
            "text/plain",
            "Failed to save settings."
        );
        return;
    }

    server.sendHeader("Location", "/settings");
    server.send(303, "text/plain", "");
}