# ESP32-Ruuvitag-Collector
This software can be used to collect measurement data from Ruuvitag Bluetooth Low Energy devices https://ruuvi.com/

Main functionalities are:
- Influx database sending
- MQTT publishing
- MQTT Automatic discovery for Home Assistant
- Store measurements to ESP32 own SPIF File System
- Use as a primary weather station data collector by sending measuments in regular intervals
- Use as a backup weather station data collector with low energy consumption
- Ruuvitag white list, collect measurements from listed Ruuvitags only
- Print and send data from offline SPIF File storage

## Mandatory network configurations
All configurations are in `config.hpp` file

WiFi SSD name

`wiFiSSD="MyHomeWifiAP"`

WiFi SSD password

`wiFiPassword="sweetHome"`

NTP Server, use value 216.239.35.0 if you dont' know any better one. Well functioning NTP server is important for good operation of this software. Use IP address, not name.

`ntpServerIP="216.239.35.0"`

## Optional network configurations
These configurations provide sending data to outside servers
### Influx servers
Currently only HTTP with username and password is supported.

Influx measurement name, also known as table name

`influxMeasurement="rm"`

Influx server IP address. Use IP address, not name.

`iC.host="192.168.1.100"`

Influx server port.

`iC.port=8086`

Influx database name

`iC.database="test0"`

Influx database username for writing

`iC.username="thewriter"`

and password

`iC.password="iamwriter"`

Add Influx server to configuration. If you don't have any Influx server, make sure below command is commented.

`influxConfiguration.push_back(iC);`

Optionally you can add an other server by specifying all or some of the new configuration.

2nd Influx server IP address. Use IP address, not name.

`iC.host="1.2.3.4"`

Add 2nd Influx server to configuration. Other than IP configuration will be same as in 1st server.     `

`influxConfiguration.push_back(iC);`

### MQTT Server
MQTT server also known as broker IP address. Set to "" to disable MQTT.

`mqttServerIP="192.168.1.100"`

and port number

`mqttServerPort=1883`

Topic name this ESP32 is publishing. The complete topic will be mqttTopicPrefix/+Ruuvitag mac address/+state. Example: ruuviesp32/C2CA7F7D07F5/state

`mqttTopicPrefix="ruuviesp32"`

Username for MQTT server

`mqttServerUsername="thepublisher"`

and password

`mqttServerPassword="iamthepublisher"`

Home Assistant can detect Ruuvitags automatically using MQTT discovery
Enter name of the topic or leave empty to disable MQTT discovery

`mqttHomeAssistantDiscoveryTopic="homeassistant"`

## Data collection settings

Ruuvitag white list. You can put one or several Ruuvitag mac addresses to white list. Then data is collected only from those Ruuvitags. Even you have only one Ruuvitag, you should still put it into a white list because this shortens Bluetooth scan time. Once all white listed devices are scanned, Bluetooth is turned off and this way energy is saved.

At the beginning start with empty white list

`macWhiteList={}`

And when you know your MAC addresses, add them to white list. You can see the addresses from serial console, Influx database or any MQTT client subscribing the topic, example Home Assistant.
Put one or more MAC addresses without any : marks as below.

`macWhiteList={"D96BF9D2116A","C2CA7F7D07F5"}`

### Primary weather station data collector
With these settings Ruuvitag data is collected every minute or 60 times per hour and it is attempted to collect at the beginning of each minute. The data is sent to Influx and MQTT servers immediately.

Wake up from deep sleep at second 0, example 12:01:00, 12:02:00, 12:03:00 etc.

`deepSleepWakeUpAtSecond=60`

You can adjust wake up to example at second 58 by setting value -2. Then the wake-up happens at 12:00:58, 12:01:58, 12:02:58 etc.

`deepSleepWakeUpOffset=0`
 
 As a primary weather station data collector we want to turn on WiFi every time ESP32 wakes up. Value 0 does that.
 
 `turnOnWifiEvery=0`
 
 ### Backup weather station data collector
With these settings Ruuvitag data is collected every minute or 60 times per hour and it is attempted to collect at the beginning of each minute. All collected data is sent to Influx servers every 15 minutes, that is 15 measurement reports. MQTT is updated only every 15 minutes.

Wake up from deep sleep at second 0, example 12:01:00, 12:02:00, 12:03:00 etc.

`deepSleepWakeUpAtSecond=60`

You can adjust wake up to example at second 58 by setting value -2. Then the wake-up happens at 12:00:58, 12:01:58, 12:02:58 etc.

`deepSleepWakeUpOffset=0`
 
As a backup weather station data collector we want to save energy and WiFi is turned on only every 15 minutes. This saves energy and allows running on battery power for much longer time than with primary weather station settings. You can set also longer period example value 7200 would turn  WiFi ON only every 2 hours. But note that ESP32 clock may slip many seconds already in two hours and NTP time is updated only when WiFi is turned ON.

 `turnOnWifiEvery=900`
 
### Additional settings

Column names for Influx. With value true the column names will be like temperature, humidty, pressure etc. and with value false the column names will be t, h, p etc.

`longColumnNames=true`

Time zone is used only for console and does not impact to data sent to Influx. If you time zone is example UTC+3, set timeZone to value UTC-3. More details here https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html

`timeZone="UTC-3"`

## Managing offline SPIF File System Storage and files
When you reset ESP32 and it successfully connects to WiFi you have a possibility to enter menu from serial console by pressing any number key. Note that you have only 2 seconds time to do so. If you fail, just reset and try again. In menu you have various options to print and send files to Influx database. You can also delete files and format the entire SPIFFS.

## Compiling
This software requires PlatformIO environment for compiling. Easiest way to get it is perhaps using Visual Studio Code and its PlatformIO add on. Use your favorite search engine to find out more on setting up PlatformIO.

## Testing
Don't have a Ruuvitag but still want to try? You can use your Android device to emulate Ruuvitag.

- Get nRF Connect app from Google Play Store
- Start App and go to Advertiser tab
- Select + from bottom right corner
- For Display name put Ruuvitag Emulator
- Choose Add Record - Manufacturer Data
- For 16 bit Company Identifier put 0499
- For Data put 0501AE1E23C75000CC008003B870F663D9A1F109FC2FF596
- Turn On Ruuvitag Emulator on Advertiser tab and you should see measument coming

## License
MIT License is used for this software.
