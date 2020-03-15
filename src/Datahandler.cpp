// Datahandler.cpp - Part of ESP32 Ruuvitag Collector
// Hannu Pirila 2019-2020
#include "Datahandler.hpp"
#include "storage.hpp"
#include "network.hpp"

Datahandler::Datahandler(std::string dataIn, std::string macAddressIn){
    dataLength=dataIn.length();
    if(dataLength>2){
        data=dataIn;
        macAddress=macAddressIn;
        if(data[0]==0x99 && data[1]==0x04){
            if(data[2]==0x3 && dataLength>15){
                measurement.setType(Measurement::ruuviV3);
            }
            if(data[2]==0x5 && dataLength>19){
                measurement.setType(Measurement::ruuviV5);
            }
        }
    }
}

Datahandler::~Datahandler(){
}

void Datahandler::setTime(){
    time(&epoch);
}

void Datahandler::setTime(time_t timeIn){
    epoch=timeIn;
}

void Datahandler::buildMeasurement(){
    if(measurement.measurementType==Measurement::undefined){
        return;
    }
    measurement.setTime(epoch);
    measurement.build(data);
}

void Datahandler::prepareStorage(){
    if(storageData.size()==0){
        uint8_t epochSize=sizeof(epoch);
        std::stringstream stream;
        global::RecordHeader recordHeader;
        if(measurement.measurementType==Measurement::ruuviV5){
            recordHeader.recordLength=dataLength-6;
        }else{
            recordHeader.recordLength=dataLength;
        }
        recordHeader.recordLength+=epochSize;
        if(measurement.measurementType==Measurement::ruuviV3){
            recordHeader.recordType=0;
        }else if(measurement.measurementType==Measurement::ruuviV5){
            recordHeader.recordType=1;
        }else{
            recordHeader.recordType=7;
        }
        stream.write(reinterpret_cast<const char*>(&recordHeader), sizeof(recordHeader));
        stream.write(reinterpret_cast<const char*>(&epoch), epochSize);
        stream << data.substr(0,recordHeader.recordLength-epochSize);
        storageData=stream.str();
    }
    if(storeFileName.size()==0){
        std::stringstream stream;
        stream << epoch-(epoch % 100000);
        stream << '_';
        stream << macAddress;
        storeFileName=stream.str();
    }
}

void Datahandler::writeStorage(){
    if(measurement.measurementType==Measurement::undefined){
        return;
    }

    prepareStorage();
    storage::write(storeFileName,storageData);
    Serial.print(macAddress.c_str());
    Serial.print(" t=");
    Serial.println(measurement.getTemperature());
}

void Datahandler::writeInflux(){
    std::stringstream stream;

    if(measurement.measurementType==Measurement::undefined){
        return;
    }

    if(network::isConnected){
        stream << config::influxMeasurement << ",";
        stream << "mac=" << macAddress << " ";
        stream << measurement.toString() << " ";
        stream << epoch;
        Serial.println("Writing a single row to Influx");
        if(network::influx::write(stream.str())){
            if(storage::spif::exists(macAddress)){
                if(storage::spif::read(macAddress,storage::OutputType::influx)){
                    storage::spif::deleteFile(macAddress);
                }else{
                    global::successfulRun=false;
                }
            }
        }else{
            storage::spif::write(macAddress,storageData);
            global::successfulRun=false;
        }
    }else{
        storage::spif::write(macAddress,storageData);
    }
}

void Datahandler::sendMqtt(){
    std::string topic;
    std::string payload;

    if(measurement.measurementType==Measurement::undefined){
        return;
    }

    if(global::bootCount<2 || global::bootCount % 7 == 0){
        network::mqtt::publishDiscovery(macAddress);
    }

    std::stringstream stream;
    stream << config::mqttTopicPrefix << "/";
    stream << macAddress << "/state";
    topic=stream.str();
    stream.str(std::string());
    stream.clear();

    stream << "{\"temperature\":" << measurement.getTemperature() << ",";
    stream << "\"humidity\":" << measurement.getHumidity() << ",";
    stream << "\"pressure\":" << measurement.getPressure() << ",";
    stream << "\"battery\":" << (double)measurement.getVoltage()/1000 << "}";
    payload=stream.str();
    network::mqtt::publish(topic,payload);
}

std::string Datahandler::toString(){
    if(measurement.measurementType!=Measurement::undefined){
        return measurement.toString();
    }else{
        return "Measurement undefined";
    }
}

void Datahandler::printData(){
    Serial.print("MAC: ");
    Serial.print(macAddress.c_str());
    Serial.print(" length: ");
    Serial.println(dataLength);

	std::stringstream stream;
    for(auto i:data){
		stream << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << (int)i << ' ';
    }
    Serial.println(stream.str().c_str());
}
