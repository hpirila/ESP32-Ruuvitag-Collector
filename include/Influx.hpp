// Influx.hpp - Part of ESP32 Ruuvitag Collector
// Hannu Pirila 2019-2020
// Original idea from https://github.com/teebr/Influx-Arduino
#ifndef Influx_hpp
#define Influx_hpp

#include <HTTPClient.h>
#include <sstream>

class Influx{
    private:
        std::string database;
        std::string host;
        int port;
        std::string username;
        std::string password;
        std::string certificate;
        std::string measurement;
        bool authorised;
        bool secure;
    public:
        Influx();
        ~Influx();
        void configure(std::string databaseIn, std::string hostIn, int portIn);
        void configure(std::string databaseIn, std::string hostIn);
        void authorize(std::string usernameIn, std::string passwordIn);
        void addCertificate(std::string certificateIn);
        int write(std::string data);
        int getResponse();
        bool isSecure();
};

#endif
