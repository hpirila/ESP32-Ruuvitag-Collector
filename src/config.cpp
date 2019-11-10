// config.cpp - Part of ESP32 Ruuvitag Collector
// Hannu Pirila 2019
#include "config.hpp"

namespace config{
    // Wifi access point name
    const std::string wiFiSSD="MyHomeWifiAP";
    // and password
    const std::string wiFiPassword="sweethome";
    // NTP server, use IP address instead of name
    const std::string ntpServerIP="216.239.35.0";

    // Only MAC values listed here are scanned or empty list will scan everything. 
    // Example: {"D96BF9D2116A","C2CA7F7D07F5"} will scan only these two MAC's
    // Example: {} empty list, everything is scanned. Start with empty list
    // and set MAC's to white list once you know the values. The system works more
    // efficiently with white list as bluetooth scan is stopped when list is fulfilled.
    std::vector<std::string>macWhiteList={};

    // Influx database settings
    // Influx measurement a.k.a table name
    std::string influxMeasurement="rm";
    std::vector<InfluxConfiguration> influxConfiguration;

    // Set influx configurations below. 
    // You can specify zero, one or many servers.
    // Each server adds up time for Influx operations, therefore
    // limit the number of servers to one or two.
    void setValues(){
        InfluxConfiguration iC;
        // Influx server IP
        iC.host="192.168.1.100";
        // Influx server port
        iC.port=8086;
        // Influx database name
        iC.database="test0";
        // Influx database username for writing
        iC.username="thewriter";
        // The username's password
        iC.password="iamwriter";
        // Add Influx server to configuration        
        influxConfiguration.push_back(iC);

        //iC.host="1.2.3.4";
        //iC.database="test0";
        //iC.username="thewriter";
        //iC.password="iamwriter";
        // Add 2nd influx server to configuration
        //influxConfiguration.push_back(iC);
    }

    // MQTT configuration
    // MQTT server a.k.a broker IP address
    std::string mqttServerIP="192.168.1.100";
    // and port number
    int mqttServerPort=1883;
    // The topic name this ESP32 is publishing
    // The complete topic will be mqttTopicPrefix/+Ruuvitag mac address/+state
    // Example: ruuviesp32/C2CA7F7D07F5/state
    std::string mqttTopicPrefix="ruuviesp32";
    // Username to connect MQTT server
    std::string mqttServerUsername="thepublisher";
    // and password
    std::string mqttServerPassword="iamthepublisher";
    // Home Assistant can detect Ruuvitags automatically using MQTT discovery
    // Enter name of the topic or leave empty to disable MQTT discovery
    std::string mqttHomeAssistantDiscoveryTopic="homeassistant";

    // timeZone: The sign is positive if the local time zone is west of the Prime Meridian and negative if it is east.
    const std::string timeZone="UTC-3";

    // How often Ruuvitag measurements are collected.
    // Example value  60 causes wake-up at 12:00:00, 12:01:00, 12:02:00... and   ~50 s deep sleep.
    // Example value 120 causes wake-up at 12:00:00, 12:02:00, 12:04:00... and  ~110 s deep sleep.
    // Do not set value below 30 as there is maybe not enough time to handle
    // all the collected data in shorter time periods.
    const int deepSleepWakeUpAtSecond=60;
    // Example value -2 causes wake-up at 12:00:58, 12:01:58, 12:02:58
    const int deepSleepWakeUpOffset=0;
    
    // How often WiFi is turned ON. 
    // Example value 3600 turns WiFi on every hour, value 7200 every two hour etc.
    // Value 0 causes WiFi to turn on everyt time the ESP wakes from deep sleep. 
    // Set value zero for real-time data reporting and higher values for to save 
    // energy. All collected data while WiFi was OFF is flushed to Influx once WiFi
    // is ON next time. Also all other network related opertaions like MQTT and and
    // NTP are run only when WiFi is ON.
    const int turnOnWifiEvery=900;

    // Watchdog will reset the ESP32 if no action in specified time
    const int watchdogTimeout=60;

    // Column names are used in report formats, example 'temperature' is long format and 't' is short format
    bool longColumnNames=true;


    // Use SD card
    bool useSDCard=false;
    // Flashlight LED is flashing during SD card write in this ESP32 Cam module
    // It requires special handling
    bool moduleIsESP32Cam=false;

    // No more settings below 
}

namespace global{

    BLEScan* pBLEScan;
    const int BLEscanTime=10;
    RTC_DATA_ATTR int bootCount=1;
    std::vector<Influx> influx;
    const uint64_t nSToSFactor=1000000000;
    const uint64_t uSToSFactor=1000000;
    const uint64_t mSToSFactor=1000;
    const int maxRowsInInfluxWrite=10;
    bool successfulRun=true;
    const std::vector<RecordHeader> validHeaders={RecordHeader{20,0},RecordHeader{24,1}};

}
