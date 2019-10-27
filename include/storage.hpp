// storage.hpp - Part of ESP32 Ruuvitag Collector
// Hannu Pirila 2019
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

  namespace spif{
    bool read(std::string fileName,OutputType outputType);
    void readAllFiles(OutputType outputType);
    void write(std::string fileName,std::string data);
    void deleteFile(std::string fileName);
    void format();
    void begin();
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
    bool read(std::string fileName,OutputType outputType);
    void readAllFiles(OutputType outputType);
    void write(std::string fileName,std::string data);
    boolean exists(std::string fileName);
    void begin();
    void list();
    uint64_t getTotalBytes();
    uint64_t getUsedBytes();
    uint64_t getFreeBytes();
  }
}





#endif