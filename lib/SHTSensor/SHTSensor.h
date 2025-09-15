#ifndef SHTSENSOR_H
#define SHTSENSOR_H

#include <Arduino.h>
#include <Wire.h>

class SHTSensor {
public:
    SHTSensor(uint8_t address = 0x44);
    bool begin(int sda_pin = -1, int scl_pin = -1);
    bool readData();
    float getTemperature();
    float getHumidity();
    bool isConnected();
    
private:
    uint8_t _address;
    float _temperature;
    float _humidity;
    bool _lastReadSuccess;
    
    bool sendCommand(uint16_t command);
    uint8_t calculateCRC(uint8_t data1, uint8_t data2);
};

#endif // SHTSENSOR_H