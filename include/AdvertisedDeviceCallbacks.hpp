// AdvertisedDeviceCallbacks.hpp - Part of ESP32 Ruuvitag Collector
// Hannu Pirila 2019
#ifndef AdvertisedDeviceCallbacks_hpp
#define AdvertisedDeviceCallbacks_hpp

#include <BLEDevice.h>
#include "config.hpp"
#include "Datahandler.hpp"
#include <sstream>
#include <iomanip>
#include <vector>

class AdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks{
private:
  int numberOfWhiteListedResults {0};
  std::string buildMac(BLEAddress bleAddress);
public:
  void onResult(BLEAdvertisedDevice advertisedDevice);
};

#endif
