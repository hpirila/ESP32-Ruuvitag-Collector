// timer.cpp - Part of ESP32 Ruuvitag Collector
// Hannu Pirila 2019
#include "timer.hpp"

namespace timer{
    namespace{
        RTC_DATA_ATTR int successfulWifiTime=0;
        RTC_DATA_ATTR int wifiNeeded=0;
        hw_timer_t *timer = NULL;
        time_t now;
        const int checkTime=1500000000;

        int roundDown(int valueIn){
            return valueIn/config::turnOnWifiEvery*config::turnOnWifiEvery;
        }
    }

    void printLocalTime(){
        struct tm timeInfo;

        time(&now);
        setenv("TZ", config::timeZone.c_str(), 1);
        tzset();

        if(!getLocalTime(&timeInfo)){
            Serial.println("Failed to obtain time");
            return;
        }
        char buffer[80];
        strftime(buffer,sizeof(buffer),"%A, %B %d %Y %H:%M:%S",&timeInfo);
        Serial.println(buffer);
    }

    namespace watchdog{
        void IRAM_ATTR resetModule() {
            Serial.println("Reset due to watchdog\n");
            esp_restart();
        }
        void feed(){
            timerWrite(timer, 0);
        }
        void set(){
            timer = timerBegin(0, 80, true);
            timerAttachInterrupt(timer, &resetModule,true);
            timerAlarmWrite(timer, config::watchdogTimeout * global::uSToSFactor, false);
            timerAlarmEnable(timer);
        }
    }
    namespace deepsleep{
        void updateBootCount(){
            global::bootCount++;
        }

        void printBootCount(){
            Serial.println();
            Serial.print("Boot number: ");
            Serial.println(global::bootCount);
        }

        void start(){
            time(&now);
            int sleepTime=config::deepSleepWakeUpAtSecond - now % config::deepSleepWakeUpAtSecond + config::deepSleepWakeUpOffset;
            esp_sleep_enable_timer_wakeup(sleepTime*global::uSToSFactor);
            Serial.print("Deep sleep for ");
            Serial.print(sleepTime);
            Serial.println(" seconds starting");
            Serial.flush();
            esp_deep_sleep_start();  
        }
    }
    namespace wifi{
        bool wiFiRequested=false;
        bool isWifiNeeded(){
            time(&now);
            if(global::bootCount<2){
                wiFiRequested=true;
                return true;
            }
            if(global::bootCount<5 && now<checkTime){
                wiFiRequested=true;
                return true;
            }
            if(config::turnOnWifiEvery==0){
                wiFiRequested=true;
                return true;
            }
            if(wifiNeeded){
                if(wifiNeeded==global::bootCount-1||global::bootCount % 10){
                    wiFiRequested=true;
                    return true;
                }else{
                    wiFiRequested=false;
                    return false;
                }
            }
            if((roundDown(now)-roundDown(successfulWifiTime))>=config::turnOnWifiEvery){
                wiFiRequested=true;
                return true;
            }
            return false;
        }
        void updateWifiRequirements(){
            if(wiFiRequested){
                if(global::successfulRun){
                    wifiNeeded=0;
                    successfulWifiTime=now;
                }else{
                    wifiNeeded=global::bootCount;
                }
            }else{
                wifiNeeded=0;
            }
        }
    }
}
