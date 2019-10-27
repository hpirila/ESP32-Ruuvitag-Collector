// menu.cpp - Part of ESP32 Ruuvitag Collector
// Hannu Pirila 2019
#include "menu.hpp"
#include "network.hpp"
#include "storage.hpp"
#include "timer.hpp"

namespace menu{
    namespace{
        std::string watchDogWarning="You need to input something every 60 seconds or watchdog resets";
        std::string lineSeparator="------------------------";
        std::string lineSeparator2="\t"+lineSeparator;
        
        void mainMenu(){
            Serial.println();
            Serial.println("MAIN MENU");
            Serial.println(lineSeparator.c_str());
            Serial.println("(1) Read files from internal SPIF File System");
            Serial.println("(2) Read files from SD card");
            Serial.println("(3) Manage internal SPIF File System");
            Serial.println("(9) Exit");
            Serial.println(lineSeparator.c_str());
            Serial.println(watchDogWarning.c_str());
            Serial.println();
        }

        void subMenu12(){
            Serial.println(lineSeparator.c_str());
            Serial.println("(1) List files");
            Serial.println("(2) Print one file,  long  column names");
            Serial.println("(3) Print all files, long  column names");
            Serial.println("(4) Print one file,  short column names");
            Serial.println("(5) Print all files, short column names");
            Serial.println("(6) Send one file to Influx");
            Serial.println("(7) Send all files to Influx");
            Serial.println("(9) Exit");
            Serial.println(lineSeparator.c_str());
            Serial.println();
        }
        
        void subMenu1(){
            Serial.println();
            Serial.println("Read files from internal SPIF File System");
            subMenu12();
        }

        void subMenu2(){
            Serial.println();
            Serial.print("Read files from SD card");
            if(storage::sd::sdMounted){
                Serial.println();
                subMenu12();
            }else{
                Serial.println(" (SD card is not present)");
                Serial.println(lineSeparator.c_str());
                Serial.println("(9) Exit");
                Serial.println(lineSeparator.c_str());
            }
        }

        void subMenu3(){
            Serial.println();
            Serial.println("Manage internal SPIF File System");
            Serial.println(lineSeparator.c_str());
            Serial.println("(1) List files");
            Serial.println("(2) Delete file");
            Serial.println("(3) Format file system");
            Serial.println("(9) Exit");
            Serial.println(lineSeparator.c_str());
            Serial.println();
        }

        char inputCharacter(std::string choices){
            std::stringstream stream;

            timer::watchdog::feed();
            
            Serial.print("Choose ");
            std::vector<char> vChoices(choices.begin(), choices.end());
            for(int i=0;i<vChoices.size();i++){
                stream << vChoices[i];
                if(vChoices.size()-i>2){
                    stream << ", ";   
                }
                if(i==vChoices.size()-2){
                    stream << " or ";   
                }
            }
            Serial.print(stream.str().c_str());
            Serial.print(": ");

            while(true){
                if(Serial.available()>0){
                    int character=Serial.read();
                    if(choices.find(toupper((char)character))!= std::string::npos){
                        Serial.println((char)character);
                        return (char)character;
                    }
                }
            }
            return 255;
        }

        std::string inputString(std::string message){
            std::stringstream stream;
            Serial.print(message.c_str());
            while(true){
                if(Serial.available()>0){
                    int ch=Serial.read();
                    if(ch==10||ch==13){
                        break;
                    }
                    stream << (char)ch;
                    Serial.print((char)ch);
                }
            }
            return stream.str();
        }

        void printOneFile(storage::FileSystemType fsType,bool longOrShortColumns,storage::OutputType outputType){
            std::string fileName;
            bool currentColumnNames;
            fileName=inputString("Type file name: ");
            Serial.println();
            Serial.println();
            if(fileName.length()>0){
                bool exists=false;

                if(fsType==storage::spiffs){
                    if(storage::spif::exists(fileName)){
                        exists=true;
                    }
                }else if (fsType==storage::sd_mmc){
                    if(storage::sd::exists(fileName)){
                        exists=true;
                    }
                }
                if(exists){
                    currentColumnNames=config::longColumnNames;
                    config::longColumnNames=longOrShortColumns;
                    if(fsType==storage::spiffs){
                        storage::spif::read(fileName,outputType);
                    }else if (fsType==storage::sd_mmc){
                        storage::sd::read(fileName,outputType);
                    }
                    config::longColumnNames=currentColumnNames;
                }else{
                    Serial.println("Error! File does not exist");
                }
            }
        }

        void deleteFile(){
            std::string fileName;
            fileName=inputString("Type file name: ");
            Serial.println();
            Serial.println();
            if(fileName.length()>0){
                if(storage::spif::exists(fileName)){
                    storage::spif::deleteFile(fileName);
                }else{
                    Serial.println("Error! File does not exist");
                }
            }
        }

        void printAllFiles(storage::FileSystemType fsType,bool longOrShortColumns,storage::OutputType outputType){
            bool currentColumnNames;
            currentColumnNames=config::longColumnNames;
            config::longColumnNames=longOrShortColumns;
            if(fsType==storage::spiffs){
                storage::spif::readAllFiles(outputType);
            }else if(fsType==storage::sd_mmc){
                storage::sd::readAllFiles(outputType);              
            }
            config::longColumnNames=currentColumnNames;
        }

        void wiFiError(){
            Serial.println("\nError! WiFi is not connected");             
        }

        void readSelection(storage::FileSystemType fsType, char selection){
            switch(selection){
                case '1':
                    if(fsType==storage::spiffs){
                        storage::spif::list();                        
                    }else if(fsType==storage::sd_mmc){
                        storage::sd::list();
                    }
                    break;
                case '2':
                    printOneFile(fsType,true,storage::print);
                    break;
                case '3':
                    printAllFiles(fsType,true,storage::print);
                    break;
                case '4':
                    printOneFile(fsType,false,storage::print);
                    break;
                case '5':
                    printAllFiles(fsType,false,storage::print);
                    break;
                case '6':
                    if(WiFi.isConnected()){
                        printOneFile(fsType,config::longColumnNames,storage::influx);
                    }else{
                        wiFiError();                                   
                    }
                    break;
                case '7':
                    if(WiFi.isConnected()){
                        printAllFiles(fsType,config::longColumnNames,storage::influx);
                    }else{
                        wiFiError();   
                    }
                    break;
                case '9':
                default:
                    break;
            }            
        }

        int menuAction(char digit){
            switch(digit){
                case '1':
                    char selection;
                    selection=255;                    
                    while(selection!='9'){
                        subMenu1();
                        selection=inputCharacter("12345679");
                        readSelection(storage::FileSystemType::spiffs,selection);
                    }
                    break;
                case '2':
                    selection=255;                    
                    while(selection!='9'){
                        subMenu2();
                        if(storage::sd::sdMounted){
                            selection=inputCharacter("12345679");
                        }else{
                            selection=inputCharacter("9");
                        }
                        readSelection(storage::FileSystemType::sd_mmc,selection);
                    }
                    break;
                case '3':
                    selection=255;                    
                    while(selection!='9'){
                        subMenu3();
                        selection=inputCharacter("1239");
                        switch(selection){
                            case '1':
                                storage::spif::list();
                                break;
                            case '2':
                                deleteFile();
                                break;
                            case '3':
                                Serial.println("Confirm SPIFFS File System Format?");
                                selection=inputCharacter("YN");
                                if(selection=='Y'||selection=='y'){
                                    storage::spif::format();
                                }
                                break;
                            case '9':
                            default:
                                break;
                        }
                    }
                    break;
                case '9':
                    return '9';
                    break;      
                default:
                    Serial.println("!WRONG INPUT!");
                    break;
            }
            mainMenu();
            return digit;
        }

        void _menu(){
            boolean inputReceived=false;
            int menuStart=millis();
            Serial.println("Press any number key open menu. You have 2 seconds time... (Note: May need number+<Enter>..");
            while(Serial.available()==0){
                if(!inputReceived && millis()-menuStart>2000){
                    break;
                }
            }
            int digit=Serial.read();
            if(digit>=48 && digit<=57){
                inputReceived=true;
                mainMenu();
            }
            delay(200);
            Serial.flush();
            while(inputReceived){
                if(menuAction(inputCharacter("1239"))=='9'){
                    break;
                }
            }
        }
    }

    void menu(){
        if(WiFi.isConnected()){
            _menu();
        }
    }
}
