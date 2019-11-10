// network.cpp - Part of ESP32 Ruuvitag Collector
// Hannu Pirila 2019
#include "network.hpp"

namespace network {
    namespace wifi {
        void begin(){
            WiFi.begin(config::wiFiSSD.c_str(),config::wiFiPassword.c_str());
            int wifiTimeout=0;
            Serial.print("Connecting Wifi SSID: ");
            Serial.println(config::wiFiSSD.c_str());
            while (WiFi.status()!= WL_CONNECTED){
                delay(500);
                Serial.print(".");
                wifiTimeout++;
                if(wifiTimeout>15){
                    break;
                }
            }
            Serial.println();
            if(WiFi.status() == WL_CONNECTED){
                Serial.print("WiFi connected. Local IP is ");
                Serial.println(WiFi.localIP());      
            }else{
                global::successfulRun=false;
            }  
        }
    }
    namespace ntp {
        void update(){
            if(WiFi.isConnected()){
                time_t timeNow;
                Serial.print("NTP update starting... ");
                
                configTime(0,0,config::ntpServerIP.c_str());
                delay(500);
                Serial.println("NTP update completed.");
                time(&timeNow);
                if(timeNow<1500000000){
                    global::successfulRun=false;
                }
            }
        }
    }
    namespace influx {
        void begin(){
            for(int i=0;i<config::influxConfiguration.size();i++){
                global::influx.push_back(Influx());
            }
            for(auto &i:global::influx){
                config::InfluxConfiguration iC=config::influxConfiguration.back();
                i.configure(iC.database,iC.host,iC.port);
                i.authorize(iC.username,iC.password);
                config::influxConfiguration.pop_back();
            }
        }

        bool write(std::string dataIn){
            int result=true;
            if(global::influx.size()<1){
                return result;
            }
            if(WiFi.isConnected()){
                for(auto i:global::influx){
                    int writeResult=i.write(dataIn);
                    switch(writeResult){
                        case 204:
                            break;
                        case 403:
                        case 404:
                            result=false;
                            break;
                        default:
                            Serial.print("\tRe-trying write once only after 3 s\n");
                            delay(3000);
                            if(i.write(dataIn)!=204){
                                result=false;
                            }
                            break;
                    }
                }
            }else{
                Serial.println("(Error: No connection)");
                result=false;
            }
            return result;
        }
    }
    namespace mqtt {
        WiFiClient espClient;
        PubSubClient client(espClient);

        void begin(){
            if(WiFi.isConnected() && config::mqttServerIP!=std::string()){
                std::string macAddress=WiFi.macAddress().c_str();
                std::string id;
                std::stringstream stream;
                stream << "ESP32";
                stream << macAddress.substr(0,2)  << macAddress.substr(3,2);
                stream << macAddress.substr(6,2)  << macAddress.substr(9,2);
                stream << macAddress.substr(12,2) << macAddress.substr(15,2);
                client.setServer(config::mqttServerIP.c_str(),config::mqttServerPort);
                client.connect (stream.str().c_str(),config::mqttServerUsername.c_str(),config::mqttServerPassword.c_str());
            }
        }

        void publish(std::string topic,std::string payload){
            if(WiFi.isConnected() && config::mqttServerIP!=std::string()){
                client.publish(topic.c_str(),payload.c_str());
            }
        }

        void publishDiscovery(std::string mac){
            if(WiFi.isConnected() && config::mqttServerIP!=std::string() && config::mqttHomeAssistantDiscoveryTopic!=std::string()){
                struct MqttDiscovery{
                    std::string deviceClass;
                    std::string unitOfMeasurement;
                    std::string format;
                };
                std::vector<MqttDiscovery> mqttDiscovery;
                std::stringstream stream;

                mqttDiscovery.push_back(MqttDiscovery{"pressure","hPa","|float|round(1)"});
                mqttDiscovery.push_back(MqttDiscovery{"humidity","%","|float|round(1)"});
                mqttDiscovery.push_back(MqttDiscovery{"temperature","°C","|float|round(1)"});
                mqttDiscovery.push_back(MqttDiscovery{"battery","V","|float|round(2)"});

                for(auto m:mqttDiscovery){
                    std::string topic;
                    std::string payload;

                    stream << config::mqttHomeAssistantDiscoveryTopic << "/sensor/";
                    stream << mac << "/" << m.deviceClass << "/config";
                    topic=stream.str();
                    stream.str(std::string());
                    stream.clear();

                    stream << "{\"unit_of_meas\":\"" << m.unitOfMeasurement << "\",";
                    stream << "\"dev_cla\":\"" << m.deviceClass << "\",";
                    stream << "\"val_tpl\":\"{{value_json." << m.deviceClass << m.format <<"}}\",";
                    stream << "\"stat_t\":\"" << config::mqttTopicPrefix << "/" << mac << "/state\",";
                    stream << "\"name\":\"" << mac << "_" << m.deviceClass << "\",";
                    stream << "\"uniq_id\":\"" << mac << "_" << m.deviceClass << "\",";
                    stream << "\"dev\":{";
                    stream << "\"ids\":[\"" << mac << "\"],";
                    stream << "\"name\":\"Ruuvitag " << mac << "\",";
                    stream << "\"mdl\":\"Ruuvitag vX\",";
                    stream << "\"mf\":\"Ruuvi Innovations Oy\"}}";
                    payload=stream.str();
                    stream.str(std::string());
                    stream.clear();
                    client.publish(topic.c_str(),payload.c_str());
                }
            }
        }
    }
}
