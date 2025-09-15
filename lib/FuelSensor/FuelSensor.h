#ifndef FUELSENSOR_H
#define FUELSENSOR_H

#include <Arduino.h>
#include <HardwareSerial.h>

class FuelSensor {
private:
    uint8_t sensorAddress;
    HardwareSerial* serial;
    
    // Protocol constants
    static const uint8_t HEADER_REQUEST = 0x31;
    static const uint8_t HEADER_RESPONSE = 0x3E;
    static const uint8_t EVENT_READ_DATA = 0x06;
    static const uint8_t EVENT_READ_LIMITS = 0x07; // New command for reading limits
    static const uint8_t EVENT_SET_FULL = 0x08;    // Set current level as full
    static const uint8_t EVENT_SET_EMPTY = 0x09;   // Set current level as empty
    
    // Response data
    float temperature;
    uint16_t fuelValue;
    uint16_t levelMax;     // Maximum fuel level
    uint16_t levelMin;     // Minimum fuel level
    bool dataValid;
    bool limitsValid;
    
    // Internal methods
    uint8_t calculateCRC8(uint8_t* data, int length);
    bool sendRequest(uint8_t eventCode);
    bool parseResponse(uint8_t* response, int length);
    bool parseLimitsResponse(uint8_t* response, int length);
    
public:
    FuelSensor(uint8_t address = 0x01);
    
    bool begin(int txPin, int rxPin, unsigned long baudrate = 9600);
    bool readSensorData();
    bool readLimits();      // New method to read max/min levels
    bool setFullLevel();    // Set current level as full (100%)
    bool setEmptyLevel();   // Set current level as empty (0%)
    
    float getTemperature() const;
    uint16_t getFuelValue() const;
    float getFuelLiters() const;    // Fuel value * 0.1
    float getFuelPercent() const;   // Fuel value * 0.1
    uint16_t getLevelMax() const;   // Maximum fuel level
    uint16_t getLevelMin() const;   // Minimum fuel level
    float getLevelMaxLiters() const; // Level max * 0.1
    float getLevelMinLiters() const; // Level min * 0.1
    bool isDataValid() const;
    bool areLimitsValid() const;
    bool isConnected();     // Check if sensor is connected
    
    void setSensorAddress(uint8_t address);
    uint8_t getSensorAddress() const;
};

#endif