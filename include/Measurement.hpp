// Measurement.hpp - Part of ESP32 Ruuvitag Collector
// Hannu Pirila 2019-2020
#ifndef Measurement_hpp
#define Measurement_hpp

#include <Arduino.h>
#include "config.hpp"
#include <string>
#include <sstream>
#include <vector>

class Measurement{
private:
    std::string mac;
    double temperature {0.0};
    double humidity {0.0};
    double pressure {0.0};
    int accelX {0};
    int accelY {0};
    int accelZ {0};
    int voltage {0};
    time_t epoch {0};
    int power {0};
    int moveCount {0};
    int sequence {0};

    int getShort(std::vector<uint8_t> data, int index);
    int getShortone(std::vector<uint8_t> data, int index);
    unsigned int getUShort(std::vector<uint8_t> data, int index);
    unsigned int getUShortone(std::vector<uint8_t> data, int index);

public:
    enum measurementTypeT {undefined,ruuviV3,ruuviV5} measurementType;
    enum measurementFormatT {shortColumnName,longColumnName};
    Measurement();
    ~Measurement();
    void setTime(time_t epochIn);
    void setType(Measurement::measurementTypeT t);
    void build(std::string dataIn);
    void buildStorage();
    double getTemperature();
    double getHumidity();
    double getPressure();
    int getVoltage();

    std::string toString();
    std::string toStr(Measurement::measurementFormatT measurementFormat);
};

#endif
