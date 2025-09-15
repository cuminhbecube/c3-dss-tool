#include "SHTSensor.h"

SHTSensor::SHTSensor(uint8_t address) {
    _address = address;
    _temperature = 0.0;
    _humidity = 0.0;
    _lastReadSuccess = false;
}

bool SHTSensor::begin(int sda_pin, int scl_pin) {
    if (sda_pin != -1 && scl_pin != -1) {
        Wire.begin(sda_pin, scl_pin);
    } else {
        Wire.begin();
    }
    
    Wire.setClock(100000); // 100kHz I2C clock
    
    // Test connection
    Wire.beginTransmission(_address);
    return (Wire.endTransmission() == 0);
}

bool SHTSensor::sendCommand(uint16_t command) {
    Wire.beginTransmission(_address);
    Wire.write(command >> 8);   // MSB
    Wire.write(command & 0xFF); // LSB
    return (Wire.endTransmission() == 0);
}

uint8_t SHTSensor::calculateCRC(uint8_t data1, uint8_t data2) {
    uint8_t crc = 0xFF;
    uint8_t data[2] = {data1, data2};
    
    for (int i = 0; i < 2; i++) {
        crc ^= data[i];
        for (int bit = 8; bit > 0; --bit) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31;
            } else {
                crc = (crc << 1);
            }
        }
    }
    return crc;
}

bool SHTSensor::readData() {
    // Send measurement command (high repeatability)
    if (!sendCommand(0x2400)) {
        _lastReadSuccess = false;
        return false;
    }
    
    delay(15); // Wait for measurement
    
    // Request 6 bytes (temp + humidity with CRC)
    Wire.requestFrom(_address, (uint8_t)6);
    
    if (Wire.available() != 6) {
        _lastReadSuccess = false;
        return false;
    }
    
    // Read temperature
    uint8_t tempMSB = Wire.read();
    uint8_t tempLSB = Wire.read();
    uint8_t tempCRC = Wire.read();
    
    // Read humidity
    uint8_t humMSB = Wire.read();
    uint8_t humLSB = Wire.read();
    uint8_t humCRC = Wire.read();
    
    // Verify CRC
    if (calculateCRC(tempMSB, tempLSB) != tempCRC || 
        calculateCRC(humMSB, humLSB) != humCRC) {
        _lastReadSuccess = false;
        return false;
    }
    
    // Convert raw data to temperature and humidity
    uint16_t tempRaw = (tempMSB << 8) | tempLSB;
    uint16_t humRaw = (humMSB << 8) | humLSB;
    
    _temperature = -45.0 + 175.0 * ((float)tempRaw / 65535.0);
    _humidity = 100.0 * ((float)humRaw / 65535.0);
    
    _lastReadSuccess = true;
    return true;
}

float SHTSensor::getTemperature() {
    return _temperature;
}

float SHTSensor::getHumidity() {
    return _humidity;
}

bool SHTSensor::isConnected() {
    Wire.beginTransmission(_address);
    return (Wire.endTransmission() == 0);
}