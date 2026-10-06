# boat-monitoring
Boat monitoring software for ESP32 controller
Setup: 
Seeed XIAO ESP32C3
2x SHT31 temp/hum sensors (inside and outside air) (I2C connection)
connected to JK BMS (UART connection)


Function:
The sensor has 2 functions. 
When the boat is unoccupied (mode OFFLINE), the power budget is limited. Measurements every 10 minutes, store values.
Every hour, try connecting to WiFi and send measurements to home server (Domoticz), write stored values to flash mem.
In between measurements, go to deep sleep.
When the boat is occupied (mode ONLINE), the power budget is much bigger. Measurements every few seconds (10?).
Still keep storing 10 minute data points, upload every hour. Keep connected to on-board wifi network (if available).
Start minimal webserver with a page to change settings (eg. WiFi credentials), some debugging (status), and maybe 
BT connection.

System modes:

OFFLINE:
- Low power
- Deep sleep
- Measurement interval: 10 min
- WiFi attempt: 1x/hour

ONLINE:
- No deep sleep (or shorter sleep)
- Fast measurements
- Local web interface
- WiFi always available if possible

functions:
Setup (initialize and self test)
Loop (general loop of the sensor)
read_env_data (read temp/hum from inside and outside sensors)
read_BMS_data (read battery data, eg. volt, SOC, etc.)
store_data_point (store the values in RTC memory)
write_data (store data points in RTC mem to flash mem)
wifi connect (try to connect to public WiFi or on board WiFi network, depending on mode)
send_data (send the data to the home server, if confirm, store last succesfull upload)
sync_time (if connected to wifi, sync time of the sensor)
purge_data (delete data if more than 2 weeks old and if succesfully uploaded)

Version 0.1:
OFFLINE MODE:
* One sensor reliable connected (SHT31) via I2C.
* 6 measurements are stored in RTC mem buffer, after that the buffer is flushed to flash storage
* wifi connection enabled
* time sync enabled
* reliable upload to InfluxDB database (once every 6 measurements)
