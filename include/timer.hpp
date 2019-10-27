// timer.hpp - Part of ESP32 Ruuvitag Collector
// Hannu Pirila 2019
#ifndef timer_hpp
#define timer_hpp
#include <Arduino.h>
#include <esp_system.h>
#include <time.h>
#include "config.hpp"
#include <sstream>

namespace timer{
    void printLocalTime();
    namespace watchdog{
        void feed();
        void set();
    }
    namespace deepsleep{
        void updateBootCount();
        int getBootCount();
        void start();
        void printBootCount();
    }
    namespace wifi{
        bool isWifiNeeded();
        void updateWifiRequirements();        
    }
}

#endif
