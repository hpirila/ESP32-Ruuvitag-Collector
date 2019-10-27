// storage.cpp - Part of ESP32 Ruuvitag Collector
// Hannu Pirila 2019
#include "storage.hpp"
#include "network.hpp"
#include "timer.hpp"

namespace storage{
    namespace{
        File file;

        std::string _getOldestFile(fs::FS &fs){
            std::stringstream stream;
            int oldestFileTimeStamp=INT_MAX;
            std::string oldestFileName;
            File root = fs.open("/");
            if(!root){
                return "";
            }
            if(!root.isDirectory()){
                return "";
            }

            File file = root.openNextFile();
            while(file){
                int fileTimeStamp;
                stream << file.name();
                fileTimeStamp=atoi( stream.str().substr(1).c_str() );
                if(fileTimeStamp < oldestFileTimeStamp && stream.str().find('_')!=std::string::npos){
                    oldestFileTimeStamp=fileTimeStamp;
                    oldestFileName=stream.str();
                }
                stream.str(std::string());
                stream.clear();
                file = root.openNextFile();
            } 
            return oldestFileName;
        }

        void _list(fs::FS &fs){
            std::stringstream stream;
            int sizeSum=0;

            File root = fs.open("/");
            if(!root){
                return;
            }
            if(!root.isDirectory()){
                return;
            }
            Serial.println("Filename                          Size");
            Serial.println("--------------------------------  --------");
                        
            File file = root.openNextFile();
            while(file){
                int fileSize=file.size();
                sizeSum+=fileSize;
                stream << std::left  << std::setw(32) << file.name() << "  ";
                stream << std::right << std::setw( 8) << fileSize;
                Serial.println(stream.str().c_str());
                stream.str(std::string());
                stream.clear();
                file = root.openNextFile();
            } 
            Serial.println("--------------------------------  --------");
            stream << std::left  << std::setw(32) << "Total:" << "  ";
            stream << std::right << std::setw( 8) << sizeSum;
            Serial.println(stream.str().c_str());
        }

        std::string _buildFileName(std::string fileName){
            if(fileName.length()>0){
                if(fileName.at(0)!='/'){
                    fileName="/"+fileName;
                }
            }            
            return fileName;            
        }

        bool _open(fs::FS &fs,std::string fsType,std::string fileName, std::string mode){
            fileName=_buildFileName(fileName);
            file=fs.open(fileName.c_str(),mode.c_str());
            if(!file){
                Serial.print(fsType.c_str());
                Serial.print(": Failed to open file ");
                Serial.print(fileName.c_str());
                Serial.print(" for ");
                Serial.println(mode.c_str());
                return false;
            }
            return true;
        }
        void _close(){
            file.close();
        }



        bool validateHeader(global::RecordHeader recordHeader){
            bool rvalue=false;
            for(auto i:global::validHeaders){
                if(recordHeader==i){
                    rvalue=true;
                    break;
                }
            }
            return rvalue;
        }
        
        std::vector<uint8_t> _readRecord(){
            global::RecordHeader recordHeader;
            std::vector<uint8_t> vData;
            int recordHeaderSize=sizeof(recordHeader);
            int fileSize=file.size();

            timer::watchdog::feed();

            if(file.position()+recordHeaderSize<=fileSize){
                file.read(reinterpret_cast <uint8_t*> (&recordHeader),recordHeaderSize);
                file.seek(file.position()-recordHeaderSize);
            }
            if(!validateHeader(recordHeader)){
                return vData;
            }
            int recordLength=recordHeader.recordLength+recordHeaderSize;
            if(fileSize-file.position()-recordLength>=0){
                vData.resize(recordLength);
                file.read(reinterpret_cast <uint8_t*> (vData.data()),recordLength);
            }
            return vData;
        }

        bool _readFile(fs::FS &fs,std::string fsType,std::string fileName,OutputType outputType){
         	std::string mac="ABBADABBAD11";
            std::stringstream stream;
            int rowCounter=0;
            int result=true;

            if(fileName.find('_')!=std::string::npos){
                mac=fileName.substr(fileName.find('_')+1,fileName.length());
            }else if(fileName.length()==mac.length()){
                mac=fileName;
            }else if(fileName.length()==mac.length()+1){
                mac=fileName.substr(fileName.find('/')+1,fileName.length());
            }

            int recordHeaderSize=sizeof(global::RecordHeader);
            int epochSize=sizeof(time_t);

            if(!_open(fs,fsType,fileName,"r")){
                return false;
            }

            while (true){
                std::vector<uint8_t> record=_readRecord();
                if(record.size()<recordHeaderSize){
                    break;
                }
                global::RecordHeader recordHeader=reinterpret_cast<const global::RecordHeader*>(&record[0])[0];
                if(recordHeader.recordLength<1){
                    break;
                }
                time_t epoch=reinterpret_cast<const time_t*>(&record[recordHeaderSize])[0];
                std::string data(reinterpret_cast<const char*>(&record[recordHeaderSize+epochSize]), recordHeader.recordLength-epochSize);

                if(recordHeader.recordType>=0){
                    Datahandler dh=Datahandler(data,mac);
                    dh.setTime(epoch);
                    dh.buildMeasurement();
                    stream << config::influxMeasurement << ",";
                    stream << "mac=" << mac << " ";
                    stream << dh.toString() << " ";
                    stream << epoch;
                    switch (outputType){
                        case OutputType::print:
                            Serial.println(stream.str().c_str());
                            stream.str(std::string());
                            stream.clear();
                            break;                        
                        case OutputType::influx:
                            stream << "\n";
                            rowCounter++;
                            if(rowCounter % global::maxRowsInInfluxWrite == 0){
                                Serial.print("Writing ");
                                Serial.print(global::maxRowsInInfluxWrite);
                                Serial.println(" rows to Influx");
                                if(!network::influx::write(stream.str())){
                                    result=false;
                                }
                                stream.str(std::string());
                                stream.clear();
                            }
                            break;                        
                        default:
                            break;
                    }
                }
            }
            if(rowCounter % global::maxRowsInInfluxWrite>0){
                Serial.print("Writing ");
                Serial.print(rowCounter % global::maxRowsInInfluxWrite);
                Serial.println(" rows to Influx");
                if(!network::influx::write(stream.str())){
                    result=false;
                }
            }
            _close();
            return result;
        }

        void _readAllFiles(fs::FS &fs,std::string fsType,OutputType outputType){
            File root = fs.open("/");
            if(!root){
                return;
            }
            if(!root.isDirectory()){
                return;
            }

            File file = root.openNextFile();
            while(file){
                Serial.println(file.name());
                _readFile(fs,fsType,file.name(),outputType);
                file = root.openNextFile();
            } 
        }


        void _write(fs::FS &fs,std::string fsType,std::string fileName,std::string data){
            fileName=_buildFileName(fileName);
            std::vector<uint8_t> vData(data.begin(),data.end());
            File file=fs.open(fileName.c_str(),FILE_APPEND);
            if(!file){
                Serial.print(fsType.c_str());
                Serial.print(": Failed to open file ");
                Serial.print(fileName.c_str());
                Serial.println(" for writing");
                return;
            }
            if(!file.write(&vData[0],data.size())){
                Serial.print(fsType.c_str());
                Serial.print(": Failed to append file ");
                Serial.println(fileName.c_str());
            }
            file.close();                    
        }

        void _deleteFile(fs::FS &fs,std::string fileName){
            fileName=_buildFileName(fileName);
            if(fs.exists(fileName.c_str())){
                Serial.printf("Deleting file: %s\r\n", fileName.c_str());
                if(fs.remove(fileName.c_str())){
                    Serial.println("- file deleted");
                } else {
                    Serial.println("- delete failed");
                }
            }
        } 

        void _deleteOldestFile(fs::FS &fs){
            _deleteFile(fs,_getOldestFile(fs));
        }

        bool _exists(fs::FS &fs,std::string fileName){
            fileName=_buildFileName(fileName);
            if(fs.exists(fileName.c_str())){
                return true;
            }else{
                return false;
            }
        } 
    }

    namespace spif{
        std::string fsType="SPIFFS";
        void begin(){
            SPIFFS.begin(true);
            SPIFFS.end();
            if(!SPIFFS.begin()){
                Serial.println("SPIFFS init failed");
                return;
            }
        }

        bool read(std::string fileName, OutputType outputType){
           return _readFile(SPIFFS,fsType,fileName,outputType);
        }

        void readAllFiles(OutputType outputType){
            _readAllFiles(SPIFFS,fsType,outputType);
        }

        void write(std::string fileName,std::string data){
            _write(SPIFFS,fsType,fileName,data);
        }

        void deleteFile(std::string fileName){
            _deleteFile(SPIFFS,fileName);
        }

        void list(){
            _list(SPIFFS);
        }

        void deleteOldestFile(){
            _deleteOldestFile(SPIFFS);
        }

        boolean exists(std::string fileName){
            return _exists(SPIFFS,fileName);
        }

        uint32_t getTotalBytes(){
            return SPIFFS.totalBytes();
        }

        uint32_t getUsedBytes(){
            return SPIFFS.usedBytes();
        }

        uint32_t getFreeBytes(){
            return SPIFFS.totalBytes()-SPIFFS.usedBytes();
        }

        void format(){
            SPIFFS.format();
            if(SPIFFS.begin()){
                Serial.println("SPIFFS format successful");
            }else{
                Serial.println("SPIFFS format failed");
            }
        }
    }

    namespace sd{
        std::string fsType="SD_MMC";
        bool sdMounted=false;
        bool read(std::string fileName, OutputType outputType){
            if(sdMounted){
                return _readFile(SD_MMC,fsType,fileName,outputType);
            }
            return false;
        }

        void readAllFiles(OutputType outputType){
            if(sdMounted){
                _readAllFiles(SD_MMC,fsType,outputType);
            }
        }

        void begin(){
            if(!SD_MMC.begin()){
                Serial.println("SD card mount failed");
                return;
            }
            sdMounted=true;
        }

        void write(std::string fileName,std::string data){
            if(sdMounted){
                _write(SD_MMC,fsType,fileName,data);
            }
        }

        void list(){
            if(sdMounted){
                _list(SD_MMC);
            }
        }

        uint64_t getTotalBytes(){
            if(sdMounted){       
                return SD_MMC.totalBytes();
            }
            return 0;
        }

        uint64_t getUsedBytes(){
            if(sdMounted){
                return SD_MMC.usedBytes();
            }
            return 0;
        }
        
        uint64_t getFreeBytes(){
            if(sdMounted){
                return SD_MMC.totalBytes()-SD_MMC.usedBytes();
            }
            return 0;
        }
        
        boolean exists(std::string fileName){
            if(sdMounted){            
                return _exists(SD_MMC,fileName);
            }
            return false;
        }
    }
}
