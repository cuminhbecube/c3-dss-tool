# Test Protocol AoooG - Broadcast Communication

## Protocol Implementation Test

### Broadcast Request Format (31 FF 06 29)
```
Byte 0: 0x31 - Header (Request)
Byte 1: 0xFF - Address (Broadcast to all sensors)
Byte 2: 0x06 - Event (Read Fuel Data)
Byte 3: 0x29 - CRC-8/MAXIM checksum
```

### Expected Response Format
```
Byte 0: 0x3E - Header (Response)
Byte 1: 0xXX - Actual sensor address (01, 02, etc.)
Byte 2: 0x06 - Event (Read Fuel Data)
Byte 3: 0xXX - Temperature value
Byte 4-5: 0xXXXX - Fuel level (Little Endian)
Byte 6-8: Additional data (optional)
Byte N: CRC-8/MAXIM checksum
```

## Usage in Code

### 1. Broadcast Detection
```cpp
// Send broadcast request to detect any available fuel sensors
bool detected = fuelSensor.readSensorDataBroadcast();
if (detected) {
    Serial.printf("Found sensor at address: 0x%02X\n", fuelSensor.getSensorAddress());
}
```

### 2. Auto-Address Update
When a sensor responds to broadcast, the library automatically updates the sensor address for future communications.

### 3. Fallback to Specific Address
If broadcast fails, the system falls back to specific address communication.

## Test Scenarios

### Scenario 1: Single Sensor
- Send: 31 FF 06 29
- Expect: 3E 01 06 [temp] [fuel_low] [fuel_high] [crc]
- Result: Address updated to 0x01

### Scenario 2: Multiple Sensors
- Send: 31 FF 06 29
- Expect: First responding sensor's data
- Result: Address updated to responding sensor's address

### Scenario 3: No Sensors
- Send: 31 FF 06 29
- Expect: Timeout after 2 seconds
- Result: No address update, connection marked as failed

## CRC-8/MAXIM Calculation
The checksum uses CRC-8/MAXIM polynomial (0x8C) with initial value 0x00.

Example calculation for "31 FF 06":
1. Start with CRC = 0x00
2. Process each byte through CRC algorithm
3. Final result should be 0x29

## Integration Benefits
1. **Auto-Discovery**: Automatically finds sensors without knowing their addresses
2. **Multi-Sensor Support**: Can work with multiple sensors on same bus
3. **Robust Communication**: Falls back to specific addressing if needed
4. **Protocol Compliance**: Follows AoooG standard exactly