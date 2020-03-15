// storage.hpp - Part of ESP32 Ruuvitag Collector
// Hannu Pirila 2019-2020
#ifndef storage_hpp
#define storage_hpp

#include "config.hpp"
#include "Datahandler.hpp"
#include <vector>
#include <sstream>
#include <iomanip>
#include <SPIFFS.h>
#include <SD_MMC.h>

namespace storage{
  enum OutputType {print,influx};
  enum FileSystemType {spiffs,sd_mmc};
  void begin();
  void end();
  void write(std::string fileName,std::string data);
  
  namespace spif{
    void begin();
    void end();
    bool read(std::string fileName,OutputType outputType);
    void write(std::string fileName,std::string data);
    void readAllFiles(OutputType outputType);
    void deleteFile(std::string fileName);
    void format();
    void list();
    void deleteOldestFile();
    boolean exists(std::string fileName);
    void sendToInflux(std::string fileName);
    uint32_t getTotalBytes();
    uint32_t getUsedBytes();
    uint32_t getFreeBytes();
  }

  namespace sd{
    extern bool sdMounted;
    void begin();
    void end();
    bool read(std::string fileName,OutputType outputType);
    void write(std::string fileName,std::string data);
    void readAllFiles(OutputType outputType);
    boolean exists(std::string fileName);
    void list();
    uint64_t getTotalBytes();
    uint64_t getUsedBytes();
    uint64_t getFreeBytes();
  }
}





#endif