#include "FuelSensor.h"

FuelSensor::FuelSensor(uint8_t address) {
    sensorAddress = address;
    serial = &Serial1;
    temperature = 0.0;
    fuelValue = 0;
    levelMax = 0;
    levelMin = 0;
    dataValid = false;
    limitsValid = false;
}

bool FuelSensor::begin(int txPin, int rxPin, unsigned long baudrate) {
    // Khởi tạo UART1 với pins được chỉ định
    serial->begin(baudrate, SERIAL_8N1, rxPin, txPin);
    
    // Đợi UART stable
    delay(100);
    
    Serial.println("FuelSensor initialized");
    Serial.printf("UART1: TX=%d, RX=%d, Baud=%lu\n", txPin, rxPin, baudrate);
    Serial.printf("Sensor Address: 0x%02X\n", sensorAddress);
    
    return true;
}

bool FuelSensor::readSensorData() {
    if (!sendRequest(EVENT_READ_DATA)) {
        Serial.println("Failed to send request to fuel sensor");
        dataValid = false;
        return false;
    }
    
    // Đợi response
    delay(50);
    
    if (serial->available() > 0) {
        uint8_t response[16];
        int bytesRead = 0;
        
        // Đọc response với timeout
        unsigned long startTime = millis();
        while (bytesRead < sizeof(response) && (millis() - startTime) < 1000) {
            if (serial->available()) {
                response[bytesRead] = serial->read();
                bytesRead++;
                
                // Kiểm tra nếu đã đọc đủ một gói tin hoàn chỉnh
                if (bytesRead >= 9) break; // Minimum response length
            }
            delay(1);
        }
        
        if (bytesRead >= 9) {
            Serial.print("Received response: ");
            for (int i = 0; i < bytesRead; i++) {
                Serial.printf("%02X ", response[i]);
            }
            Serial.println();
            
            return parseResponse(response, bytesRead);
        } else {
            Serial.printf("Insufficient response data: %d bytes\n", bytesRead);
            dataValid = false;
            return false;
        }
    } else {
        Serial.println("No response from fuel sensor");
        dataValid = false;
        return false;
    }
}

bool FuelSensor::sendRequest(uint8_t eventCode) {
    uint8_t request[4];
    request[0] = HEADER_REQUEST;     // 0x31
    request[1] = sensorAddress;      // Sensor address
    request[2] = eventCode;          // Event code (0x06 for data, 0x07 for limits)
    request[3] = calculateCRC8(request, 3); // CRC
    
    Serial.print("Sending request: ");
    for (int i = 0; i < 4; i++) {
        Serial.printf("%02X ", request[i]);
    }
    Serial.println();
    
    // Clear any existing data in buffer
    while (serial->available()) {
        serial->read();
    }
    
    // Send request
    serial->write(request, 4);
    serial->flush();
    
    return true;
}

bool FuelSensor::parseResponse(uint8_t* response, int length) {
    if (length < 9) {
        Serial.println("Response too short");
        return false;
    }
    
    // Kiểm tra header
    if (response[0] != HEADER_RESPONSE) {
        Serial.printf("Invalid header: 0x%02X (expected 0x%02X)\n", response[0], HEADER_RESPONSE);
        return false;
    }
    
    // Kiểm tra address
    if (response[1] != sensorAddress) {
        Serial.printf("Address mismatch: 0x%02X (expected 0x%02X)\n", response[1], sensorAddress);
        return false;
    }
    
    // Kiểm tra event code
    if (response[2] != EVENT_READ_DATA) {
        Serial.printf("Event code mismatch: 0x%02X (expected 0x%02X)\n", response[2], EVENT_READ_DATA);
        return false;
    }
    
    // Tính toán và kiểm tra CRC
    uint8_t calculatedCRC = calculateCRC8(response, length - 1);
    if (response[length - 1] != calculatedCRC) {
        Serial.printf("CRC mismatch: 0x%02X (calculated 0x%02X)\n", response[length - 1], calculatedCRC);
        // Vẫn tiếp tục parse data, chỉ warning
    }
    
    // Parse temperature (byte 3)
    temperature = (float)response[3];
    
    // Parse fuel value (bytes 4-5, little endian)
    fuelValue = (uint16_t)response[4] | ((uint16_t)response[5] << 8);
    
    Serial.printf("Parsed - Temperature: %.1f°C, Fuel Value: %d\n", temperature, fuelValue);
    
    dataValid = true;
    return true;
}

uint8_t FuelSensor::calculateCRC8(uint8_t* data, int length) {
    // CRC-8/MAXIM implementation
    uint8_t crc = 0x00;
    
    for (int i = 0; i < length; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x01) {
                crc = (crc >> 1) ^ 0x8C;
            } else {
                crc >>= 1;
            }
        }
    }
    
    return crc;
}

float FuelSensor::getTemperature() const {
    return temperature;
}

uint16_t FuelSensor::getFuelValue() const {
    return fuelValue;
}

float FuelSensor::getFuelLiters() const {
    return fuelValue * 0.1f;
}

float FuelSensor::getFuelPercent() const {
    return fuelValue * 0.1f;
}

bool FuelSensor::isDataValid() const {
    return dataValid;
}

void FuelSensor::setSensorAddress(uint8_t address) {
    if (address >= 1 && address <= 253) {
        sensorAddress = address;
        Serial.printf("Sensor address set to: 0x%02X\n", sensorAddress);
    } else {
        Serial.println("Invalid sensor address (valid range: 1-253)");
    }
}

uint8_t FuelSensor::getSensorAddress() const {
    return sensorAddress;
}

bool FuelSensor::readLimits() {
    if (!sendRequest(EVENT_READ_LIMITS)) {
        Serial.println("Failed to send limits request to fuel sensor");
        limitsValid = false;
        return false;
    }
    
    // Đợi response
    delay(50);
    
    if (serial->available() > 0) {
        uint8_t response[16];
        int bytesRead = 0;
        
        // Đọc response
        while (serial->available() && bytesRead < sizeof(response)) {
            response[bytesRead] = serial->read();
            bytesRead++;
            delay(2);
        }
        
        Serial.print("Limits Response: ");
        for (int i = 0; i < bytesRead; i++) {
            Serial.printf("%02X ", response[i]);
        }
        Serial.println();
        
        if (bytesRead >= 7) {
            return parseLimitsResponse(response, bytesRead);
        } else {
            Serial.printf("Insufficient limits response data: %d bytes\n", bytesRead);
            limitsValid = false;
            return false;
        }
    } else {
        Serial.println("No limits response from fuel sensor");
        limitsValid = false;
        return false;
    }
}

bool FuelSensor::parseLimitsResponse(uint8_t* response, int length) {
    if (length < 7) {
        Serial.println("Limits response too short");
        return false;
    }
    
    // Kiểm tra header
    if (response[0] != HEADER_RESPONSE) {
        Serial.printf("Invalid limits header: 0x%02X (expected 0x%02X)\n", response[0], HEADER_RESPONSE);
        return false;
    }
    
    // Kiểm tra address
    if (response[1] != sensorAddress) {
        Serial.printf("Limits address mismatch: 0x%02X (expected 0x%02X)\n", response[1], sensorAddress);
        return false;
    }
    
    // Kiểm tra event code
    if (response[2] != EVENT_READ_LIMITS) {
        Serial.printf("Limits event code mismatch: 0x%02X (expected 0x%02X)\n", response[2], EVENT_READ_LIMITS);
        return false;
    }
    
    // Parse level max (bytes 3-4, little endian)
    levelMax = (uint16_t)response[3] | ((uint16_t)response[4] << 8);
    
    // Parse level min (bytes 5-6, little endian)
    levelMin = (uint16_t)response[5] | ((uint16_t)response[6] << 8);
    
    Serial.printf("Parsed Limits - Max: %d, Min: %d\n", levelMax, levelMin);
    
    limitsValid = true;
    return true;
}

uint16_t FuelSensor::getLevelMax() const {
    return levelMax;
}

uint16_t FuelSensor::getLevelMin() const {
    return levelMin;
}

float FuelSensor::getLevelMaxLiters() const {
    return levelMax * 0.1f;
}

float FuelSensor::getLevelMinLiters() const {
    return levelMin * 0.1f;
}

bool FuelSensor::areLimitsValid() const {
    return limitsValid;
}

bool FuelSensor::setFullLevel() {
    if (!sendRequest(EVENT_SET_FULL)) {
        Serial.println("Failed to send SET FULL command to fuel sensor");
        return false;
    }
    
    // Wait for response
    unsigned long startTime = millis();
    uint8_t buffer[32];
    int index = 0;
    
    while (millis() - startTime < 2000) { // 2 second timeout
        if (serial->available()) {
            buffer[index] = serial->read();
            index++;
            
            if (index >= 6) { // Minimum response length
                // Simple check for successful response (0x3E header)
                if (buffer[0] == HEADER_RESPONSE) {
                    Serial.println("SET FULL command successful");
                    // Re-read limits to update local values
                    delay(100);
                    readLimits();
                    return true;
                }
                index = 0; // Reset if invalid header
            }
        }
        delay(10);
    }
    
    Serial.println("SET FULL command timeout");
    return false;
}

bool FuelSensor::setEmptyLevel() {
    if (!sendRequest(EVENT_SET_EMPTY)) {
        Serial.println("Failed to send SET EMPTY command to fuel sensor");
        return false;
    }
    
    // Wait for response
    unsigned long startTime = millis();
    uint8_t buffer[32];
    int index = 0;
    
    while (millis() - startTime < 2000) { // 2 second timeout
        if (serial->available()) {
            buffer[index] = serial->read();
            index++;
            
            if (index >= 6) { // Minimum response length
                // Simple check for successful response (0x3E header)
                if (buffer[0] == HEADER_RESPONSE) {
                    Serial.println("SET EMPTY command successful");
                    // Re-read limits to update local values
                    delay(100);
                    readLimits();
                    return true;
                }
                index = 0; // Reset if invalid header
            }
        }
        delay(10);
    }
    
    Serial.println("SET EMPTY command timeout");
    return false;
}

bool FuelSensor::isConnected() {
    // Quick connectivity check by sending a read data request
    if (!sendRequest(EVENT_READ_DATA)) {
        return false;
    }
    
    // Wait for any response (even if invalid)
    unsigned long startTime = millis();
    while (millis() - startTime < 500) { // 500ms timeout
        if (serial->available()) {
            // Clear buffer and return true if we got any response
            while (serial->available()) {
                serial->read();
            }
            return true;
        }
        delay(10);
    }
    
    return false; // No response - sensor disconnected
}