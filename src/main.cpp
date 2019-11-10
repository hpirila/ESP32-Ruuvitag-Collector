// main.cpp - Part of ESP32 Ruuvitag Collector
// Hannu Pirila 2019
#include <Arduino.h>
#include "config.hpp"
#include "AdvertisedDeviceCallbacks.hpp"
#include "storage.hpp"
#include "network.hpp"
#include "menu.hpp"
#include "timer.hpp"

void setup() {
  Serial.begin(115200);
  timer::watchdog::set();
  timer::watchdog::feed();
  timer::deepsleep::printBootCount();

  config::setValues();

  if(timer::wifi::isWifiNeeded()){
    network::wifi::begin();
    network::ntp::update();
  }

  timer::printLocalTime();

  network::influx::begin();
  storage::begin();

  menu::menu();  
  if(storage::spif::getFreeBytes()<8192){
    storage::spif::deleteOldestFile();
  }
  network::mqtt::begin();

  BLEDevice::init("");
  global::pBLEScan = BLEDevice::getScan();
  global::pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
  global::pBLEScan->setActiveScan(false);
  BLEScanResults foundDevices = global::pBLEScan->start(global::BLEscanTime);

  storage::end();
  timer::wifi::updateWifiRequirements();
  timer::deepsleep::updateBootCount();
  timer::deepsleep::start();
}

void loop() {
}
