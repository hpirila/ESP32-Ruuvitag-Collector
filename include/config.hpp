// config.hpp - Part of ESP32 Ruuvitag Collector
// Hannu Pirila 2019-2020
#ifndef config_h
#define config_h
#include <string>
#include <vector>
#include "Influx.hpp"
#include <esp_system.h>
#include <BLEDevice.h>
#define DEBUG_ERROR
#define DEBUG_INFORMATION

// See setting descriptions in config.cpp where you can also set values
// Do not set any values here.
namespace config{
    extern const bool ethernetInUse;
    extern const bool ethernetUseFixedIp;
    extern const bool ethernetFallbackToWiFi;
    extern const IPAddress ethernetFixedIPAddress;
    extern const IPAddress ethernetFixedIPNetworkMask;
    extern const IPAddress ethernetFixedIPGateway;

    extern const std::string wiFiSSD;
    extern const std::string wiFiPassword;
    extern const std::string ntpServerIP;
    extern bool longColumnNames;

    extern const int watchdogTimeout;
    extern const int deepSleepWakeUpAtSecond;
    extern const int deepSleepWakeUpOffset;
    extern const std::string timeZone;
    extern const int turnOnWifiEvery;

    extern std::vector<std::string>macWhiteList;
    
    struct InfluxConfiguration{
        std::string database;
        std::string username;
        std::string password;
        std::string host;
        int port;
    };
    extern std::string influxMeasurement;
    extern std::vector<InfluxConfiguration> influxConfiguration;
    
    extern std::string mqttTopicPrefix;
    extern std::string mqttServerIP;
    extern int mqttServerPort;
    extern std::string mqttServerUsername;
    extern std::string mqttServerPassword;
    extern std::string mqttHomeAssistantDiscoveryTopic;
    extern bool useSDCard;
    extern bool moduleIsESP32Cam;

    void setValues();

}
namespace global{

    extern const uint64_t nSToSFactor;
    extern const uint64_t uSToSFactor;
    extern const uint64_t mSToSFactor;

    extern const int maxRowsInInfluxWrite;
    extern std::vector<Influx> influx;

    struct RecordHeader{
        bool operator==(const RecordHeader &other)const{
            return recordLength==other.recordLength&&recordType==other.recordType;
        }
        bool operator!=(const RecordHeader &other)const{
            return recordLength!=other.recordLength||recordType!=other.recordType;
        }         
        uint8_t recordLength:5;
        uint8_t recordType:3;
    };
    extern RTC_DATA_ATTR int bootCount;
    extern BLEScan* pBLEScan;
    extern const int BLEscanTime;
    extern const std::vector<RecordHeader>validHeaders;
    extern int bootCount;
    extern bool successfulRun;
}
#endif
