#ifndef FUELSENSOR_H
#define FUELSENSOR_H

#include <Arduino.h>
#include <HardwareSerial.h>

class FuelSensor {
private:
    uint8_t sensorAddress;
    HardwareSerial* serial;
    
    // Protocol constants - AoooG Protocol
    static const uint8_t HEADER_REQUEST = 0x31;
    static const uint8_t HEADER_RESPONSE = 0x3E;
    static const uint8_t BROADCAST_ADDRESS = 0xFF; // Broadcast address for all sensors
    static const uint8_t EVENT_READ_DATA = 0x06;
    static const uint8_t EVENT_READ_LIMITS = 0x07; // New command for reading limits
    static const uint8_t EVENT_SET_FULL = 0x08;    // Set current level as full (OLD)
    static const uint8_t EVENT_SET_EMPTY = 0x09;   // Set current level as empty (OLD)
    static const int RESPONSE_DELAY = 100;         // Response delay in milliseconds
    
    // New Full/Empty frequency commands (require A1 prefix)
    static const uint8_t EVENT_SET_EMPTY_FREQ = 0x45;   // Set Empty Frequency
    static const uint8_t EVENT_SET_FULL_FREQ = 0x46;    // Set Full Frequency
    static const uint8_t EVENT_READ_EMPTY_FREQ = 0x51;  // Read Empty Frequency
    
    // Extended/Hidden commands
    static const uint8_t EVENT_READ_FIRMWARE = 0x1C; // Read firmware version
    static const uint8_t EVENT_READ_SERIAL = 0x02; // Read serial number
    static const uint8_t EVENT_FACTORY_RESET = 0x18; // Factory reset command
    static const uint8_t EVENT_EXTENDED_E3 = 0xE3; // Extended/hidden command
    static const uint8_t EVENT_RESTART_SENSOR = 0x4D; // Restart sensor (broadcast)
    
    // Response data
    float temperature;
    uint16_t fuelValue;
    uint16_t frequency;        // Current frequency value from readSensorData
    uint16_t levelMax;     // Maximum fuel level
    uint16_t levelMin;     // Minimum fuel level
    bool dataValid;
    bool limitsValid;
    
    // Extended command responses
    uint8_t firmwareVersion[32];   // Firmware version buffer (increased size)
    int firmwareVersionLength;     // Firmware version length
    uint8_t serialNumberData[8];   // Serial number response buffer
    int serialNumberLength;        // Serial number response length
    uint32_t serialNumber;         // Parsed serial number
    uint8_t extendedResponse[32];  // Buffer for extended responses
    int extendedResponseLength;    // Length of extended response
    
    // Raw data storage
    uint8_t lastRawResponse[16];  // Store last raw response
    int lastRawResponseLength;     // Length of last raw response
    String lastRawDataString;      // Formatted hex string of raw data
    
    // Set command response data
    uint8_t lastSetResponse[8];    // Store last Set command response
    int lastSetResponseLength;     // Length of last Set response
    String lastSetCommand;         // Name of last Set command executed
    bool lastSetSuccess;           // Success status of last Set command
    
    // Empty frequency data
    uint16_t emptyFrequency;       // Empty frequency value (little-endian from response)
    bool emptyFrequencyValid;      // Whether empty frequency data is valid
    
    // Internal methods
    uint8_t calculateCRC8(uint8_t* data, int length);
    bool sendRequest(uint8_t eventCode);
    bool parseResponse(uint8_t* response, int length);
    bool parseBroadcastResponse(uint8_t* response, int length);
    bool parseLimitsResponse(uint8_t* response, int length);
    bool readExtendedResponse(const char* commandName); // Helper for extended commands
    
public:
    FuelSensor(uint8_t address = 0x01);
    
    bool begin(int txPin, int rxPin, unsigned long baudrate = 9600);
    bool readSensorData();
    bool readSensorDataBroadcast(); // New method for broadcast reading
    bool readLimits();      // New method to read max/min levels
    bool setFullLevel();    // Set current level as full (100%) - NEW SEQUENCE
    bool setEmptyLevel();   // Set current level as empty (0%) - NEW SEQUENCE
    bool readEmptyFrequency(); // Read empty frequency (31 01 51 [CRC])
    bool isConnected();     // Check if sensor is connected
    
    // Extended commands
    bool readFirmwareVersion();    // Read firmware version (31 FF 1C CA)
    bool readSerialNumber();       // Read serial number (31 FF 02 [CRC])
    bool factoryReset();           // Factory reset (31 FF 18 [CRC])
    bool sendExtendedE3();         // Send extended E3 command (31 FF E3 FF)
    bool restartSensor();          // Restart sensor (31 FF 4D 4F)
    bool sendMultipleCommands();   // Send multiple commands in sequence
    
    // Getters for extended data
    const uint8_t* getFirmwareVersion() const;
    int getFirmwareVersionLength() const;
    const uint8_t* getSerialNumberData() const;
    int getSerialNumberLength() const;
    uint32_t getSerialNumber() const;
    const uint8_t* getExtendedResponse() const;
    int getExtendedResponseLength() const;
    
    float getTemperature() const;
    uint16_t getFuelValue() const;
    uint16_t getFrequency() const;      // Current frequency value
    float getFuelLiters() const;    // Fuel value * 0.1
    float getFuelPercent() const;   // Fuel value * 0.1
    uint16_t getLevelMax() const;   // Maximum fuel level
    uint16_t getLevelMin() const;   // Minimum fuel level
    float getLevelMaxLiters() const; // Level max * 0.1
    float getLevelMinLiters() const; // Level min * 0.1
    bool isDataValid() const;
    bool areLimitsValid() const;
    
    // Empty frequency access methods
    uint16_t getEmptyFrequency() const;     // Get empty frequency value
    bool isEmptyFrequencyValid() const;     // Check if empty frequency is valid
    
    // Raw data access methods
    String getLastRawData() const;  // Get formatted raw data string
    const uint8_t* getLastRawResponse() const; // Get raw response array
    int getLastRawResponseLength() const;      // Get raw response length
    
    // Set command response access methods
    const uint8_t* getLastSetResponse() const; // Get last Set command response
    int getLastSetResponseLength() const;      // Get last Set response length
    String getLastSetCommand() const;          // Get last Set command name
    bool getLastSetSuccess() const;            // Get last Set command success status
    String getLastSetResponseString() const;   // Get formatted Set response string
    
    void setSensorAddress(uint8_t address);
    uint8_t getSensorAddress() const;
};

#endif