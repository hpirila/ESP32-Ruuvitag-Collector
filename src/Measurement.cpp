// Measurement.cpp - Part of ESP32 Ruuvitag Collector
// Hannu Pirila 2019
#include "Measurement.hpp"

using namespace std;

Measurement::Measurement(){
    measurementType=Measurement::undefined;
}

Measurement::~Measurement(){
}

void Measurement::setTime(time_t epochIn){
    epoch=epochIn;
}

void Measurement::build(string dataIn){
    vector<uint8_t> data(dataIn.begin(),dataIn.end());
    switch (measurementType){
        case Measurement::ruuviV3:
            temperature=  (double)(getUShortone(data, 4) & 0b01111111) + (double)getUShortone(data, 5)/100;
            temperature=  (getUShortone(data, 4) & 0b10000000)==128 ? temperature*-1 : temperature;
            humidity=     (double)getUShortone(data, 3) * 0.5;
            pressure=     (double)getUShort(data, 6)/100+500;
            accelX=       getShort(data,8);
            accelY=       getShort(data,10);
            accelZ=       getShort(data,12);
            voltage=      (short)getUShort(data, 14);
            break;
        case Measurement::ruuviV5:
            temperature=  (double)getShort (data,3)*0.005;
            humidity=     (double)getUShort(data,5)*0.0025;
            pressure=     (double)getUShort(data,7)/100+500;
            accelX=       getShort(data,9);
            accelY=       getShort(data,11);
            accelZ=       getShort(data,13);
            voltage=      (data[15] << 3 | data[16] >> 5)+1600;
            power=        (data[16] & 0x1F)*2-40;
            moveCount=    getUShortone(data, 17);
            sequence=     getUShort(data, 18);
            break;
        default:
            break; 
    }
}

string Measurement::toString(){
    if(config::longColumnNames){
        return toStr(Measurement::longColumnName);
    }else{
        return toStr(Measurement::shortColumnName);
    }
}

std::string Measurement::toStr(Measurement::measurementFormatT measurementFormat){
    stringstream stream;
    stringstream sTemperature;
    stringstream sPressure;
    stringstream sHumidity;
    stringstream sAccelX;
    stringstream sAccelY;
    stringstream sAccelZ;
    stringstream sVoltage;
    stringstream sEpoch;
    stringstream sPower;
    stringstream sMovecount;
    stringstream sSequence;

    if(measurementFormat==Measurement::longColumnName){
        sTemperature << "temperature=" << temperature;
        sPressure    << "pressure="    << pressure;
        sHumidity    << "humidity="    << humidity;
        sAccelX      << "accelX="      << accelX;
        sAccelY      << "accelY="      << accelY;
        sAccelZ      << "accelZ="      << accelZ;
        sVoltage     << "battery="     << voltage;
        sEpoch       << "epoch="       << epoch;
        sPower       << "txdbm="       << power;
        sMovecount   << "move="        << moveCount;
        sSequence    << "sequence="    << sequence;
    }else{
        sTemperature << "t=" << temperature;
        sPressure    << "p=" << pressure;
        sHumidity    << "h=" << humidity;
        sAccelX      << "x=" << accelX;
        sAccelY      << "y=" << accelY;
        sAccelZ      << "z=" << accelZ;
        sVoltage     << "b=" << voltage;
        sEpoch       << "e=" << epoch;
        sPower       << "l=" << power;
        sMovecount   << "m=" << moveCount;
        sSequence    << "s=" << sequence;
    }
    
    stream << sTemperature.str()  << ",";
    stream << sPressure.str()     << ",";
    stream << sHumidity.str()     << ",";
    stream << sAccelX.str()       << ",";
    stream << sAccelY.str()       << ",";
    stream << sAccelZ.str()       << ",";
    stream << sVoltage.str()      << ",";
    stream << sEpoch.str();
    if(measurementType==Measurement::ruuviV5){
        stream << ",";
        stream << sPower.str()     << ",";
        stream << sMovecount.str() << ",";
        stream << sSequence.str();
    }
    return stream.str();
}

int Measurement::getShort(vector<uint8_t> data, int index){
    return (short)((data[index] << 8) + (data[index + 1]));
}

int Measurement::getShortone(vector<uint8_t> data, int index){
    return (short)((data[index]));
}

unsigned int Measurement::getUShort(vector<uint8_t> data, int index){
    return (unsigned short)((data[index] << 8) + (data[index + 1]));
}

unsigned int Measurement::getUShortone(vector<uint8_t> data, int index){
    return (unsigned short)((data[index]));
}

void Measurement::setType(Measurement::measurementTypeT t){
    measurementType=t;
}

double Measurement::getTemperature(){
    return temperature;
}

double Measurement::getHumidity(){
    return humidity;
}

double Measurement::getPressure(){
    return pressure;
}

int Measurement::getVoltage(){
    return voltage;
}

double Measurement::getAccelX(){
    return accelX;
}

double Measurement::getAccelY(){
    return accelY;
}

double Measurement::getAccelZ(){
    return accelZ;
}

int Measurement::getEpoch(){
    return epoch;
}

int Measurement::getTXdBm(){
    return power;
}

int Measurement::getMoveCount(){
    return moveCount;
}

int Measurement::getSequence(){
    return sequence;
}