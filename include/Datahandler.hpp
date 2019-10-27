// Datahandler.hpp - Part of ESP32 Ruuvitag Collector
// Hannu Pirila 2019
#ifndef Datahandler_hpp
#define Datahandler_hpp

#include <Arduino.h>
#include "Measurement.hpp"
#include "config.hpp"
#include <vector>
#include <iomanip>
#include <sstream>

class Datahandler{
  private:
    std::string data;
    uint8_t dataLength {0};
    std::string macAddress;
    time_t epoch;
    std::string storeFileName;
    Measurement measurement;
    std::string storageData;
    std::string compressedStorageData;
    void prepareStorage();

  public:
    Datahandler(std::string dataIn, std::string macAddressIn);
    ~Datahandler();
    void printData();
    void buildMeasurement();
    void writeSpif();
    void setTime();
    void setTime(time_t epochIn);
    void writeInflux();
    void sendMqtt();
    std::string toString();
};

#endif
