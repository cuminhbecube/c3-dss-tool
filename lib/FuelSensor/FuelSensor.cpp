#include "FuelSensor.h"

FuelSensor::FuelSensor(uint8_t address) {
    sensorAddress = address;
    serial = &Serial1;
    temperature = 0.0;
    fuelValue = 0;
    frequency = 0;
    levelMax = 0;
    levelMin = 0;
    dataValid = false;
    limitsValid = false;
    firmwareVersionLength = 0;
    extendedResponseLength = 0;
    lastRawResponseLength = 0;
    lastRawDataString = "";
    memset(lastRawResponse, 0, sizeof(lastRawResponse));
    memset(extendedResponse, 0, sizeof(extendedResponse));
    memset(firmwareVersion, 0, sizeof(firmwareVersion));
    
    // Initialize Set command response variables
    memset(lastSetResponse, 0, sizeof(lastSetResponse));
    lastSetResponseLength = 0;
    lastSetCommand = "";
    lastSetSuccess = false;
    
    // Initialize empty frequency variables
    emptyFrequency = 0;
    emptyFrequencyValid = false;
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

bool FuelSensor::readSensorDataBroadcast() {
    // Temporarily save current sensor address
    uint8_t originalAddress = sensorAddress;
    
    // Set to broadcast address
    sensorAddress = BROADCAST_ADDRESS; // 0xFF
    
    Serial.println("=== BROADCAST REQUEST ===");
    Serial.println("Sending broadcast request (31 FF 06 29) to all fuel sensors...");
    
    if (!sendRequest(EVENT_READ_DATA)) {
        Serial.println("Failed to send broadcast request to fuel sensors");
        sensorAddress = originalAddress; // Restore original address
        dataValid = false;
        return false;
    }
    
    // Đợi response từ bất kỳ sensor nào
    delay(100); // Increased delay for broadcast response
    
    if (serial->available() > 0) {
        uint8_t response[16];
        int bytesRead = 0;
        
        // Đọc response với timeout
        unsigned long startTime = millis();
        while (bytesRead < sizeof(response) && (millis() - startTime) < 1000) { // Increased timeout
            if (serial->available()) {
                response[bytesRead] = serial->read();
                bytesRead++;
                
                // Kiểm tra nếu đã đọc đủ một gói tin hoàn chỉnh
                if (bytesRead >= 9) break; // Minimum response length
            }
            delay(1);
        }
        
        if (bytesRead >= 9) {
            Serial.print("Broadcast response received: ");
            for (int i = 0; i < bytesRead; i++) {
                Serial.printf("%02X ", response[i]);
            }
            Serial.println();
            
            // Parse response (any sensor can respond)
            bool result = parseBroadcastResponse(response, bytesRead);
            sensorAddress = originalAddress; // Restore original address
            return result;
        } else {
            Serial.printf("Insufficient broadcast response data: %d bytes\n", bytesRead);
            sensorAddress = originalAddress; // Restore original address
            dataValid = false;
            return false;
        }
    } else {
        Serial.println("No response from any fuel sensor (broadcast)");
        sensorAddress = originalAddress; // Restore original address
        dataValid = false;
        return false;
    }
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
    request[1] = sensorAddress;      // Sensor address (0x01, 0x02... or 0xFF for broadcast)
    request[2] = eventCode;          // Event code (0x06 for data, 0x07 for limits)
    request[3] = calculateCRC8(request, 3); // CRC-8/MAXIM checksum
    
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
    // Store raw response data
    lastRawResponseLength = min(length, (int)sizeof(lastRawResponse));
    memcpy(lastRawResponse, response, lastRawResponseLength);
    
    // Create formatted hex string
    lastRawDataString = "";
    for (int i = 0; i < lastRawResponseLength; i++) {
        if (i > 0) lastRawDataString += " ";
        lastRawDataString += String(response[i], HEX);
        if (response[i] < 16) lastRawDataString = lastRawDataString.substring(0, lastRawDataString.length()-1) + "0" + lastRawDataString.substring(lastRawDataString.length()-1);
    }
    lastRawDataString.toUpperCase();
    
    if (length < 9) {
        Serial.println("Response too short");
        return false;
    }
    
    // Kiểm tra header - phải là 0x3E theo protocol AoooG
    if (response[0] != HEADER_RESPONSE) {
        Serial.printf("Invalid header: 0x%02X (expected 0x%02X)\n", response[0], HEADER_RESPONSE);
        return false;
    }
    
    // Kiểm tra address - chỉ kiểm tra nếu không phải broadcast
    if (sensorAddress != BROADCAST_ADDRESS && response[1] != sensorAddress) {
        Serial.printf("Address mismatch: 0x%02X (expected 0x%02X)\n", response[1], sensorAddress);
        return false;
    }
    
    // Kiểm tra event code
    if (response[2] != EVENT_READ_DATA) {
        Serial.printf("Event code mismatch: 0x%02X (expected 0x%02X)\n", response[2], EVENT_READ_DATA);
        return false;
    }
    
    // Tính toán và kiểm tra CRC-8/MAXIM
    uint8_t calculatedCRC = calculateCRC8(response, length - 1);
    if (response[length - 1] != calculatedCRC) {
        Serial.printf("CRC mismatch: 0x%02X (calculated 0x%02X)\n", response[length - 1], calculatedCRC);
        // Vẫn tiếp tục parse data, chỉ warning
    }
    
    // Parse temperature (byte 3)
    temperature = (float)response[3];
    
    // Parse fuel value (bytes 4-5, little endian)
    fuelValue = (uint16_t)response[4] | ((uint16_t)response[5] << 8);
    
    // Parse frequency (bytes 6-7, big endian: A7 E7 = 0xE7A7 = 59303 Hz) 
    if (length >= 9) {
        frequency = ((uint16_t)response[6] << 8) | (uint16_t)response[7];
    } else {
        frequency = 0;
    }
    
    Serial.printf("Parsed - Sensor: 0x%02X, Temperature: %.1f°C, Fuel Value: %d, Frequency: %d Hz\n", 
                  response[1], temperature, fuelValue, frequency);
    
    dataValid = true;
    return true;
}

bool FuelSensor::parseBroadcastResponse(uint8_t* response, int length) {
    // Store raw response data
    lastRawResponseLength = min(length, (int)sizeof(lastRawResponse));
    memcpy(lastRawResponse, response, lastRawResponseLength);
    
    // Create formatted hex string
    lastRawDataString = "";
    for (int i = 0; i < lastRawResponseLength; i++) {
        if (i > 0) lastRawDataString += " ";
        lastRawDataString += String(response[i], HEX);
        if (response[i] < 16) lastRawDataString = lastRawDataString.substring(0, lastRawDataString.length()-1) + "0" + lastRawDataString.substring(lastRawDataString.length()-1);
    }
    lastRawDataString.toUpperCase();
    
    if (length < 9) {
        Serial.println("Broadcast response too short");
        return false;
    }
    
    // Kiểm tra header - phải là 0x3E theo protocol AoooG
    if (response[0] != HEADER_RESPONSE) {
        Serial.printf("Invalid broadcast header: 0x%02X (expected 0x%02X)\n", response[0], HEADER_RESPONSE);
        return false;
    }
    
    // Không kiểm tra address trong broadcast - chấp nhận từ bất kỳ sensor nào
    uint8_t respondingSensorAddress = response[1];
    Serial.printf("Response from sensor address: 0x%02X\n", respondingSensorAddress);
    
    // Kiểm tra event code
    if (response[2] != EVENT_READ_DATA) {
        Serial.printf("Broadcast event code mismatch: 0x%02X (expected 0x%02X)\n", response[2], EVENT_READ_DATA);
        return false;
    }
    
    // Tính toán và kiểm tra CRC-8/MAXIM
    uint8_t calculatedCRC = calculateCRC8(response, length - 1);
    if (response[length - 1] != calculatedCRC) {
        Serial.printf("Broadcast CRC mismatch: 0x%02X (calculated 0x%02X)\n", response[length - 1], calculatedCRC);
        // Vẫn tiếp tục parse data, chỉ warning
    }
    
    // Parse temperature (byte 3)
    temperature = (float)response[3];
    
    // Parse fuel value (bytes 4-5, little endian)
    fuelValue = (uint16_t)response[4] | ((uint16_t)response[5] << 8);
    
    // Parse frequency (bytes 6-7, big endian: A7 E7 = 0xE7A7 = 59303 Hz)
    if (length >= 9) {
        frequency = ((uint16_t)response[6] << 8) | (uint16_t)response[7];
    } else {
        frequency = 0;
    }
    
    Serial.printf("Broadcast Parsed - Responding Sensor: 0x%02X, Temperature: %.1f°C, Fuel Value: %d, Frequency: %d Hz\n", 
                  respondingSensorAddress, temperature, fuelValue, frequency);
    
    // Cập nhật address thành sensor đã trả lời để sử dụng cho các lệnh tiếp theo
    sensorAddress = respondingSensorAddress;
    Serial.printf("Updated sensor address to: 0x%02X\n", sensorAddress);
    
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

uint16_t FuelSensor::getFrequency() const {
    return frequency;
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

uint16_t FuelSensor::getEmptyFrequency() const {
    return emptyFrequency;
}

bool FuelSensor::isEmptyFrequencyValid() const {
    return emptyFrequencyValid;
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
    Serial.println("=== SET FULL LEVEL ===");
    
    // Send Set Full Frequency command directly (31 FF 46 6F)
    Serial.println("Sending Set Full Frequency command...");
    uint8_t command[] = {0x31, 0xFF, 0x46, 0x6F}; // 31 FF 46 6F
    
    serial->write(command, sizeof(command));
    
    Serial.printf("Sent: 31 FF 46 6F\n");
    
    // Wait for response: 3E 01 46 00 80 (where 00=OK, 01=Error)
    unsigned long responseStart = millis();
    while (millis() - responseStart < RESPONSE_DELAY) {
        // Non-blocking delay for response
    }
    
    if (serial->available() >= 5) {
        uint8_t response[8];
        int bytesRead = serial->readBytes(response, min(8, serial->available()));
        
        // Store Set command response data
        lastSetResponseLength = min(bytesRead, (int)sizeof(lastSetResponse));
        memcpy(lastSetResponse, response, lastSetResponseLength);
        lastSetCommand = "SET FULL";
        
        Serial.printf("Set Full response (%d bytes): ", bytesRead);
        for (int i = 0; i < bytesRead; i++) {
            Serial.printf("0x%02X ", response[i]);
        }
        Serial.println();
        
        // Check response format: 3E 01 46 00/01 80 (where 01=OK, 00=Error)
        if (bytesRead >= 5 && response[0] == HEADER_RESPONSE && response[2] == EVENT_SET_FULL_FREQ) {
            if (response[3] == 0x01) {
                Serial.println("SET FULL successful (response: 01)");
                lastSetSuccess = true;
                // Re-read limits to update local values
                unsigned long delayStart = millis();
                while (millis() - delayStart < 100) {
                    // Non-blocking delay 100ms
                }
                readLimits();
                
                // Send restart command after 5 second delay
                Serial.println("Sending restart command after SET FULL...");
                delayStart = millis();
                while (millis() - delayStart < 5000) {
                    // Non-blocking delay 5000ms (5 seconds)
                }
                restartSensor();
                
                return true;
            } else {
                Serial.printf("SET FULL failed (response: %02X)\n", response[3]);
                lastSetSuccess = false;
                return false;
            }
        } else {
            Serial.println("Invalid Set Full response format");
            lastSetSuccess = false;
            return false;
        }
    }
    
    Serial.println("No response to Set Full command");
    lastSetSuccess = false;
    return false;
}

bool FuelSensor::setEmptyLevel() {
    Serial.println("=== SET EMPTY LEVEL ===");
    
    // Send Set Empty Frequency command directly (31 FF 45 8D)
    Serial.println("Sending Set Empty Frequency command...");
    uint8_t command[] = {0x31, 0xFF, 0x45, 0x8D}; // 31 FF 45 8D
    
    serial->write(command, sizeof(command));
    
    Serial.printf("Sent: 31 FF 45 8D\n");
    
    // Wait for response: 3E 01 45 00 80 (where 00=OK, 01=Error)
    unsigned long responseStart = millis();
    while (millis() - responseStart < RESPONSE_DELAY) {
        // Non-blocking delay for response
    }
    
    if (serial->available() >= 5) {
        uint8_t response[8];
        int bytesRead = serial->readBytes(response, min(8, serial->available()));
        
        // Store Set command response data
        lastSetResponseLength = min(bytesRead, (int)sizeof(lastSetResponse));
        memcpy(lastSetResponse, response, lastSetResponseLength);
        lastSetCommand = "SET EMPTY";
        
        Serial.printf("Set Empty response (%d bytes): ", bytesRead);
        for (int i = 0; i < bytesRead; i++) {
            Serial.printf("0x%02X ", response[i]);
        }
        Serial.println();
        
        // Check response format: 3E 01 45 00/01 80 (where 01=OK, 00=Error)
        if (bytesRead >= 5 && response[0] == HEADER_RESPONSE && response[2] == EVENT_SET_EMPTY_FREQ) {
            if (response[3] == 0x01) {
                Serial.println("SET EMPTY successful (response: 01)");
                lastSetSuccess = true;
                // Re-read limits to update local values
                unsigned long delayStart = millis();
                while (millis() - delayStart < 100) {
                    // Non-blocking delay 100ms
                }
                readLimits();
                
                // Send restart command after 5 second delay
                Serial.println("Sending restart command after SET EMPTY...");
                delayStart = millis();
                while (millis() - delayStart < 5000) {
                    // Non-blocking delay 5000ms (5 seconds)
                }
                restartSensor();
                
                return true;
            } else {
                Serial.printf("SET EMPTY failed (response: %02X)\n", response[3]);
                lastSetSuccess = false;
                return false;
            }
        } else {
            Serial.println("Invalid Set Empty response format");
            lastSetSuccess = false;
            return false;
        }
    }
    
    Serial.println("No response to Set Empty command");
    lastSetSuccess = false;
    return false;
}

bool FuelSensor::readEmptyFrequency() {
    // Build command: 31 FF 51 [CRC]
    uint8_t command[] = {0x31, 0xFF, 0x51};
    uint8_t checksum = calculateCRC8(command, sizeof(command));
    
    // Send command
    serial->write(command, sizeof(command));
    serial->write(checksum);
    
    Serial.printf("Sent Read Empty Frequency command: ");
    for (int i = 0; i < sizeof(command); i++) {
        Serial.printf("0x%02X ", command[i]);
    }
    Serial.printf("CRC: 0x%02X\n", checksum);
    
    // Wait for response
    unsigned long responseStart = millis();
    while (millis() - responseStart < RESPONSE_DELAY) {
        // Non-blocking delay for response
    }
    
    if (serial->available()) {
        uint8_t response[8];
        int bytesRead = 0;
        unsigned long startTime = millis();
        
        // Read response with timeout
        while (bytesRead < sizeof(response) && (millis() - startTime) < 1000) {
            if (serial->available()) {
                response[bytesRead] = serial->read();
                bytesRead++;
            }
        }
        
        // Log received response
        Serial.printf("Read Empty Frequency response (%d bytes): ", bytesRead);
        for (int i = 0; i < bytesRead; i++) {
            Serial.printf("0x%02X ", response[i]);
        }
        Serial.println();
        
        // Expected response: 3E 01 51 26 4A 2A (or 3E FF 51 26 4A 2A for broadcast)
        // Parse response: Header=3E, Address=01/FF, Event=51, Data=26 4A, CRC=2A
        if (bytesRead >= 6 && response[0] == 0x3E && 
            (response[1] == 0x01 || response[1] == 0xFF) && response[2] == 0x51) {
            // Extract frequency data (little-endian): 4A 26 = 0x264A = 9802
            // But according to example: 26 4A => 4A 26 = 18982
            emptyFrequency = (response[4] << 8) | response[3]; // 4A 26 format
            emptyFrequencyValid = true;
            
            Serial.printf("Empty Frequency parsed: 0x%02X%02X = %d\n", 
                         response[4], response[3], emptyFrequency);
            
            return true;
        } else {
            Serial.println("Invalid Read Empty Frequency response format");
            emptyFrequencyValid = false;
            return false;
        }
    }
    
    Serial.println("No response to Read Empty Frequency command");
    emptyFrequencyValid = false;
    return false;
}

bool FuelSensor::isConnected() {
    // Quick connectivity check by sending a broadcast request
    uint8_t originalAddress = sensorAddress;
    sensorAddress = BROADCAST_ADDRESS; // Use broadcast for detection
    
    bool connected = false;
    if (sendRequest(EVENT_READ_DATA)) {
        // Wait for any response (even if invalid)
        unsigned long startTime = millis();
        while (millis() - startTime < 500) { // 500ms timeout
            if (serial->available()) {
                // Clear buffer and return true if we got any response
                while (serial->available()) {
                    serial->read();
                }
                connected = true;
                break;
            }
            delay(10);
        }
    }
    
    sensorAddress = originalAddress; // Restore original address
    return connected;
}

// Private helper method for reading extended command responses
bool FuelSensor::readExtendedResponse(const char* commandName) {
    delay(100);
    
    if (serial->available() > 0) {
        uint8_t response[32];
        int bytesRead = 0;
        
        // Read response with timeout
        unsigned long startTime = millis();
        while (bytesRead < sizeof(response) && (millis() - startTime) < 1000) {
            if (serial->available()) {
                response[bytesRead] = serial->read();
                bytesRead++;
            }
            delay(1);
        }
        
        if (bytesRead > 0) {
            Serial.printf("%s response: ", commandName);
            for (int i = 0; i < bytesRead; i++) {
                Serial.printf("%02X ", response[i]);
            }
            Serial.println();
            
            // Store in extended response buffer
            extendedResponseLength = min(bytesRead, (int)sizeof(extendedResponse));
            memcpy(extendedResponse, response, extendedResponseLength);
            
            return true;
        }
    }
    
    Serial.printf("No response to %s command\n", commandName);
    return false;
}

// Raw data access methods
String FuelSensor::getLastRawData() const {
    return lastRawDataString;
}

const uint8_t* FuelSensor::getLastRawResponse() const {
    return lastRawResponse;
}

int FuelSensor::getLastRawResponseLength() const {
    return lastRawResponseLength;
}

bool FuelSensor::readFirmwareVersion() {
  uint8_t command[] = {0x31, 0xFF, 0x1C};
  uint8_t checksum = calculateCRC8(command, sizeof(command));
  
  // Send command with checksum
  serial->write(command, sizeof(command));
  serial->write(checksum);
  
  Serial.printf("Sent firmware read command: ");
  for (int i = 0; i < sizeof(command); i++) {
    Serial.printf("0x%02X ", command[i]);
  }
  Serial.printf("CRC: 0x%02X\n", checksum);
  
  // Wait for response
  delay(RESPONSE_DELAY);
  
  if (serial->available() >= 4) {
    uint8_t response[32];
    int available = serial->available();
    int bytesRead = serial->readBytes(response, min(32, available));
    
    // Store firmware response
    memcpy(firmwareVersion, response, min(32, bytesRead));
    firmwareVersionLength = bytesRead;
    
    Serial.printf("Firmware response (%d bytes): ", bytesRead);
    for (int i = 0; i < bytesRead; i++) {
      Serial.printf("0x%02X ", response[i]);
    }
    Serial.println();
    
    // Also print as string for debugging
    Serial.print("Firmware as string: ");
    for (int i = 0; i < bytesRead; i++) {
      if (response[i] >= 32 && response[i] <= 126) {
        Serial.print((char)response[i]);
      } else {
        Serial.printf("[%02X]", response[i]);
      }
    }
    Serial.println();
    
    return true;
  }
  
  Serial.println("No firmware response received");
  return false;
}

bool FuelSensor::readSerialNumber() {
  uint8_t command[] = {0x31, 0xFF, 0x02};
  uint8_t checksum = calculateCRC8(command, sizeof(command));
  
  // Clear any pending data
  while (serial->available()) {
    serial->read();
  }
  
  // Send command with checksum
  serial->write(command, sizeof(command));
  serial->write(checksum);
  
  Serial.printf("Sent serial number read command: ");
  for (int i = 0; i < sizeof(command); i++) {
    Serial.printf("0x%02X ", command[i]);
  }
  Serial.printf("CRC: 0x%02X\n", checksum);
  
  // Wait for response
  delay(RESPONSE_DELAY);
  
  if (serial->available() >= 8) { // Expected: 3E 01 02 3C 83 0C 00 4E (8 bytes)
    uint8_t response[8];
    int bytesRead = serial->readBytes(response, min(8, serial->available()));
    
    // Store serial number response
    memcpy(serialNumberData, response, min(8, bytesRead));
    serialNumberLength = bytesRead;
    
    Serial.printf("Serial number response (%d bytes): ", bytesRead);
    for (int i = 0; i < bytesRead; i++) {
      Serial.printf("0x%02X ", response[i]);
    }
    Serial.println();
    
    // Parse serial number from response
    if (bytesRead >= 7 && response[0] == 0x3E && response[2] == 0x02) {
      // Data is at bytes 3-6: 3C 83 0C 00 (little-endian U32)
      serialNumber = ((uint32_t)response[6] << 24) | 
                     ((uint32_t)response[5] << 16) | 
                     ((uint32_t)response[4] << 8) | 
                     ((uint32_t)response[3]);
      
      Serial.printf("Parsed serial number: %lu\n", serialNumber);
    }
    
    return true;
  }
  
  Serial.println("No serial number response received");
  return false;
}

bool FuelSensor::factoryReset() {
  uint8_t command[] = {0x31, 0xFF, 0x18};
  uint8_t checksum = calculateCRC8(command, sizeof(command));
  
  // Clear any pending data
  while (serial->available()) {
    serial->read();
  }
  
  // Send command with checksum
  serial->write(command, sizeof(command));
  serial->write(checksum);
  
  Serial.printf("Sent factory reset command: ");
  for (int i = 0; i < sizeof(command); i++) {
    Serial.printf("0x%02X ", command[i]);
  }
  Serial.printf("CRC: 0x%02X\n", checksum);
  
  // Wait for response
  delay(RESPONSE_DELAY);
  
  if (serial->available() >= 5) { // Expected: 3E 01 18 00 6C (5 bytes)
    uint8_t response[8];
    int bytesRead = serial->readBytes(response, min(8, serial->available()));
    
    // Store factory reset response
    memcpy(lastSetResponse, response, min(8, bytesRead));
    lastSetResponseLength = bytesRead;
    lastSetCommand = "FACTORY_RESET";
    
    Serial.printf("Factory reset response (%d bytes): ", bytesRead);
    for (int i = 0; i < bytesRead; i++) {
      Serial.printf("0x%02X ", response[i]);
    }
    Serial.println();
    
    // Parse response: 3E 01 18 00 6C
    if (bytesRead >= 4 && response[0] == 0x3E && response[1] == 0x01 && response[2] == 0x18) {
      if (response[3] == 0x00) {
        Serial.println("Factory reset successful (SET OK)");
        lastSetSuccess = true;
      } else {
        Serial.printf("Factory reset failed with status: 0x%02X\n", response[3]);
        lastSetSuccess = false;
      }
    } else {
      Serial.println("Invalid factory reset response format");
      lastSetSuccess = false;
    }
    
    return lastSetSuccess;
  }
  
  Serial.println("No factory reset response received");
  lastSetSuccess = false;
  return false;
}

bool FuelSensor::sendExtendedE3() {
  uint8_t command[] = {0x31, 0xFF, 0xE3};
  uint8_t checksum = calculateCRC8(command, sizeof(command));
  
  serial->write(command, sizeof(command));
  serial->write(checksum);
  
  Serial.printf("Sent extended E3 command: ");
  for (int i = 0; i < sizeof(command); i++) {
    Serial.printf("0x%02X ", command[i]);
  }
  Serial.printf("CRC: 0x%02X\n", checksum);
  
  delay(RESPONSE_DELAY);
  
  if (serial->available() >= 4) {
    uint8_t response[16];
    int bytesRead = serial->readBytes(response, min(16, serial->available()));
    
    memcpy(extendedResponse, response, min(16, bytesRead));
    extendedResponseLength = bytesRead;
    
    Serial.printf("Extended E3 response (%d bytes): ", bytesRead);
    for (int i = 0; i < bytesRead; i++) {
      Serial.printf("0x%02X ", response[i]);
    }
    Serial.println();
    
    return true;
  }
  
  Serial.println("No E3 response received");
  return false;
}

bool FuelSensor::restartSensor() {
  uint8_t command[] = {0x31, 0xFF, 0x4D, 0x0A};
  
  serial->write(command, sizeof(command));
  
  Serial.printf("Sent restart command: ");
  for (int i = 0; i < sizeof(command); i++) {
    Serial.printf("0x%02X ", command[i]);
  }
  Serial.println();
  
  // Restart command has no response expected
  unsigned long delayStart = millis();
  while (millis() - delayStart < 1000) {
    // Non-blocking delay 1000ms - wait for sensor restart
  }
  
  Serial.println("Sensor restart command sent (no response expected)");
  return true;
}

bool FuelSensor::sendMultipleCommands() {
  Serial.println("=== Sending Multiple Extended Commands ===");
  
  bool success = true;
  
  // 1. Read Firmware Version
  Serial.println("1. Reading firmware version...");
  if (readFirmwareVersion()) {
    Serial.println("   Firmware read successful");
  } else {
    Serial.println("   Firmware read failed");
    success = false;
  }
  delay(500);
  
  // 2. Send Extended E3
  Serial.println("2. Sending Extended E3 command...");
  if (sendExtendedE3()) {
    Serial.println("   E3 command successful");
  } else {
    Serial.println("   E3 command failed");
    success = false;
  }
  delay(500);
  
  // 3. Restart Sensor
  Serial.println("3. Restarting sensor...");
  if (restartSensor()) {
    Serial.println("   Restart command successful");
  } else {
    Serial.println("   Restart command failed");
    success = false;
  }
  
  Serial.printf("=== Multiple commands completed. Success: %s ===\n", success ? "YES" : "NO");
  return success;
}

const uint8_t* FuelSensor::getFirmwareVersion() const {
  return firmwareVersion;
}

int FuelSensor::getFirmwareVersionLength() const {
  return firmwareVersionLength;
}

const uint8_t* FuelSensor::getSerialNumberData() const {
  return serialNumberData;
}

int FuelSensor::getSerialNumberLength() const {
  return serialNumberLength;
}

uint32_t FuelSensor::getSerialNumber() const {
  return serialNumber;
}

const uint8_t* FuelSensor::getExtendedResponse() const {
  return extendedResponse;
}

int FuelSensor::getExtendedResponseLength() const {
  return extendedResponseLength;
}

// Set command response access methods
const uint8_t* FuelSensor::getLastSetResponse() const {
  return lastSetResponse;
}

int FuelSensor::getLastSetResponseLength() const {
  return lastSetResponseLength;
}

String FuelSensor::getLastSetCommand() const {
  return lastSetCommand;
}

bool FuelSensor::getLastSetSuccess() const {
  return lastSetSuccess;
}

String FuelSensor::getLastSetResponseString() const {
  String responseStr = "";
  for (int i = 0; i < lastSetResponseLength; i++) {
    if (i > 0) responseStr += " ";
    responseStr += String(lastSetResponse[i], HEX);
    if (lastSetResponse[i] < 16) {
      responseStr = responseStr.substring(0, responseStr.length()-1) + "0" + responseStr.substring(responseStr.length()-1);
    }
  }
  responseStr.toUpperCase();
  return responseStr;
}