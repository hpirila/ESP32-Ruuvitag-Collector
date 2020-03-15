// Influx.cpp - Part of ESP32 Ruuvitag Collector
// Hannu Pirila 2019-2020
// Original idea from https://github.com/teebr/Influx-Arduino
#include "Influx.hpp"

using namespace std;

Influx::Influx(){
    secure=false;
    authorised=false;
}
Influx::~Influx(){   
}

void Influx::configure(string databaseIn, string hostIn, int portIn){
    database = databaseIn;
    host = hostIn;
    port = portIn;
}

void Influx::configure(string databaseIn, string hostIn){
    const int influxDefaultPort=8086;
    configure(databaseIn,hostIn,influxDefaultPort);
}

void Influx::authorize(string usernameIn, string passwordIn){
    username=usernameIn;
    password=passwordIn;
    authorised=true;
}

void Influx::addCertificate(string certificateIn){
    certificate=certificateIn;
    secure = true;
}

int Influx::write(string data){
    HTTPClient http;
    string uri;
    stringstream stream;

    stream << "/write?db=" << database;
    stream << "&precision=s";
    uri=stream.str();
    stream.clear();    

    if(secure){
        http.begin(host.c_str(), port, uri.c_str(), certificate.c_str());
    }else{
        http.begin(host.c_str(), port, uri.c_str());
    }

    http.addHeader("Content-Type", "text/plain");

    if(authorised){
        http.setAuthorization(username.c_str(),password.c_str());
    }
    
    int result=http.POST(data.c_str());

    Serial.print("\tInflux write result from host ");
    Serial.print(host.c_str());
    Serial.print(": ");
    Serial.print(result);
    result==204 ? Serial.println(" (OK)") : Serial.println(" (Error)");

    http.end();
    return result;
}
