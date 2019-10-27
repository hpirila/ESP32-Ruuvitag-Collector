// AdvertisedDeviceCallbacks.cpp - Part of ESP32 Ruuvitag Collector
// Hannu Pirila 2019
#include "AdvertisedDeviceCallbacks.hpp"

using namespace std;

void AdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice){
    bool processMac=false;
    BLEAddress bleAddress=BLEAddress(advertisedDevice.getAddress());
    string data=advertisedDevice.getManufacturerData();
    string mac;

    mac=buildMac(bleAddress);

    for(auto m:config::macWhiteList){
        if(mac==m){
            numberOfWhiteListedResults++;
            processMac=true;
        }
    }
    if(config::macWhiteList.size()<1){
        processMac=true;
    }
    if(processMac){
        Datahandler dh=Datahandler(data,mac);
        dh.setTime();
        dh.buildMeasurement();
        dh.writeSpif();
        dh.writeInflux();
        dh.sendMqtt();
    }
    Serial.println("----------------------------------");
    if(config::macWhiteList.size()>0){
        if(numberOfWhiteListedResults==config::macWhiteList.size()){
            global::pBLEScan->stop();
        }
    }
}

string AdvertisedDeviceCallbacks::buildMac(BLEAddress bleAddress) {
	stringstream stream;
	for(int i=0;i<6;i++){
		stream << setfill('0') << setw(2) << uppercase << hex << (int) ((uint8_t*) (bleAddress.getNative()))[i];
	}
    return stream.str();
}