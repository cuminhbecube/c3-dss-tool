# ESP32-C3 DSS Tool - Advanced Industrial Sensor Monitor

## 🚀 Overview
High-performance IoT monitoring system built on ESP32-C3 for industrial fuel sensors and environmental monitoring with optimized OLED interface, real-time hotswap detection, and comprehensive fuel sensor protocol support.

## ✨ Key Features

### 🎛 **Advanced User Interface**
- **Instant Response**: 10-50ms encoder response time (40-200x faster)
- **Inverted Highlights**: Professional menu selection with color inversion  
- **Multi-Screen Navigation**: Fuel details with 5-page scrollable views
- **Compact Layout**: Optimized for 128x32 OLED displays
- **Clean Design**: Minimalist interface with hidden advanced settings

### 🔌 **Smart Sensor Management**
- **Hotswap Detection**: Auto-detect sensors within 5 seconds
- **Dual Sensor Support**: SHT31/35 + Industrial fuel sensor with AoooG protocol
- **Real-time Monitoring**: Continuous connectivity checking with broadcast addressing
- **Visual/Audio Feedback**: LED + buzzer notifications

### ⛽ **Advanced Fuel Sensor Support**
- **AoooG Protocol**: Full implementation with broadcast addressing (0xFF)
- **Comprehensive Commands**: Read data, limits, firmware, serial number, calibration
- **Frequency Display**: Real-time frequency monitoring from sensor
- **Auto-Discovery**: Automatic sensor address detection and configuration
- **Extended Commands**: Factory reset, restart, empty frequency reading

### 📱 **Optimized Display**
- **128x32 OLED**: Perfect layout utilization with compact information display
- **Multi-Page Views**: 5-page scrollable fuel details (Basic, Raw Data, Firmware, Serial, Status)
- **Compact Info Display**: Temperature + Units + Frequency on single line
- **Status Indicators**: Connection status, sensor health, protocol information

### 🎮 **Intuitive Controls**
- **Single Click**: Navigate between different views
- **Click on Selection**: Enter detail view with scrollable pages
- **Double Click**: Hidden calibration access
- **Long Press**: Execute calibration and advanced commands

## 🔧 Hardware Configuration

### ESP32-C3 DevKit M-1
- **MCU**: ESP32-C3 160MHz, 320KB RAM, 4MB Flash
- **Power**: 3.3V operation
- **Connectivity**: I2C, UART, GPIO interrupts

### Pin Assignments
```
┌─────────────────┬─────────────────┐
│ Component       │ GPIO Pin        │
├─────────────────┼─────────────────┤
│ OLED SDA        │ GPIO 6          │
│ OLED SCL        │ GPIO 5          │
│ Encoder SW      │ GPIO 8          │
│ Encoder DT      │ GPIO 9          │
│ Encoder CLK     │ GPIO 10         │
│ Fuel TX         │ GPIO 1          │
│ Fuel RX         │ GPIO 2          │
│ Buzzer          │ GPIO 7          │
│ LED1 (System)   │ GPIO 20         │
│ LED2 (Sensor)   │ GPIO 21         │
└─────────────────┴─────────────────┘
```

### 📺 **Display**: SSD1306 OLED 0.91"
- **Resolution**: 128x32 pixels
- **Interface**: I2C (0x3C)
- **Power**: 3.3V
- **Features**: High contrast, fast refresh

### 🌡 **SHT Sensor**: SHT31/35
- **Interface**: I2C (0x44 or 0x45)  
- **Range**: -40°C to +125°C, 0-100% RH
- **Accuracy**: ±0.3°C, ±2% RH
- **Power**: 3.3V

### ⛽ **Fuel Sensor**: Industrial AoooG Protocol
- **Protocol**: AoooG RS232 with broadcast addressing (0xFF)
- **Interface**: UART (9600 baud, 8N1)
- **Commands**: Full command set with CRC-8/MAXIM checksums
- **Data**: Temperature, fuel level, frequency, min/max limits
- **Calibration**: Set full/empty levels, factory reset, restart
- **Extended**: Firmware version, serial number, hidden commands

### 🎚 **Rotary Encoder**
- **Type**: Mechanical with button
- **Resolution**: 20-24 steps/revolution
- **Debounce**: Hardware + software (30ms)
- **Response**: Interrupt-driven

### 🔊 **Audio Feedback**
- **Buzzer**: 8530 PWM (2.7kHz)
- **Patterns**: Success, warning, error tones
- **Volume**: Software controlled

## 💻 Software Architecture

### Core Libraries
- **DisplayManager**: Advanced OLED graphics with 5-page scrollable fuel details
- **SHTSensor**: Temperature/humidity with error handling  
- **FuelSensor**: Complete AoooG protocol implementation with broadcast addressing
- **RotaryEncoder**: Anti-jitter with multi-click detection
- **BuzzerManager**: Audio patterns and feedback

### Fuel Sensor Protocol Features
- **Broadcast Commands**: All commands use 0xFF address for universal compatibility
- **Complete Command Set**: Read data, limits, firmware, serial, calibration, reset
- **CRC Validation**: CRC-8/MAXIM checksums for data integrity
- **Auto-Discovery**: Automatic sensor detection and address learning
- **Frequency Monitoring**: Real-time frequency display from sensor responses
- **Extended Commands**: Factory reset, restart, empty frequency reading

### Performance Optimizations
- **Separated Update Loops**: Sensors (2s) vs Display (50ms)
- **Force Update Mechanism**: Instant encoder feedback
- **Static Variable Caching**: Reduce computation overhead
- **Interrupt-driven Input**: Non-blocking encoder handling

## 🎯 User Interface

### Main Screen Layout
```
┌─── DSS TOOL ─────────────┐
├─────────┬─────────────────┤
│  FUEL   │      SHT        │
│   50L   │     25°C        │
│         │     60%         │
└─────────┴─────────────────┘
```

### Fuel Detail View (FUEL [1/5])
```
┌─── FUEL [1/5] ───────────┐
├─────────────────────────┤
│ T:25.0C U:150 F:59303   │
│ Max:1000 Min:0          │
└─────────────────────────┘
```

### 5-Page Fuel Details Navigation
- **Page 1/5**: Basic Info - Temperature, Units, Frequency, Max/Min limits
- **Page 2/5**: Raw Data - Complete serial response data
- **Page 3/5**: Firmware - Version information and hex data
- **Page 4/5**: Serial Number - Device serial and hex representation  
- **Page 5/5**: Status - Protocol info, connection status, percentage calculation

### Navigation Flow
```
Main Menu → [Single Click] → Detail View → [Scroll] → 5 Pages
    ↓
[Double Click] → Calibration Mode
    ↓
[Long Press] → Execute Command
```

## 🛠 Development

### Build Requirements
- **PlatformIO**: Latest version
- **Platform**: espressif32
- **Framework**: arduino
- **Board**: esp32-c3-devkitm-1

### Dependencies
```ini
lib_deps = 
    adafruit/Adafruit SSD1306@^2.5.15
    adafruit/Adafruit GFX Library@^1.12.1
    adafruit/Adafruit BusIO@^1.17.2
    bblanchon/ArduinoJson@^6.21.5
```

### Build Commands
```bash
# Compile
platformio run

# Upload
platformio run --target upload --upload-port COM5

# Monitor
platformio device monitor --port COM5 --baud 115200
```

## 📊 Performance Metrics
- **Response Time**: 10-50ms (encoder to display)
- **Sensor Update**: 2-second intervals  
- **Hotswap Detection**: 5-second intervals
- **Memory Usage**: 7.5KB RAM, 164KB Flash
- **Power Consumption**: ~80mA @ 3.3V

## 🔄 Recent Updates
- ✅ Complete AoooG protocol implementation with broadcast addressing
- ✅ Frequency display integration from sensor responses  
- ✅ 5-page scrollable fuel detail views
- ✅ Compact display layout optimized for 128x32 OLED
- ✅ Extended command support (firmware, serial, factory reset)
- ✅ Auto-restart functionality with 5-second delay
- ✅ CRC-8/MAXIM checksum validation
- ✅ Enhanced visual feedback and error handling
- ✅ Improved setFull/setEmpty response logic (01=success, 00=error)

## 📡 AoooG Protocol Implementation

### Command Structure
```
Request:  31 [ADDR] [CMD] [CRC]
Response: 3E [ADDR] [CMD] [DATA...] [CRC]
```

### Supported Commands
```
┌─────────┬──────┬────────────────────────┐
│ Command │ Code │ Description            │
├─────────┼──────┼────────────────────────┤
│ Read    │ 06   │ Read sensor data       │
│ Limits  │ 07   │ Read min/max limits    │
│ SetFull │ 46   │ Set full level         │
│ SetEmpty│ 45   │ Set empty level        │
│ ReadEmpty│ 51  │ Read empty frequency   │
│ Firmware│ 1C   │ Read firmware version  │
│ Serial  │ 02   │ Read serial number     │
│ Reset   │ 18   │ Factory reset          │
│ Restart │ 4D   │ Restart sensor         │
│ Extended│ E3   │ Extended/hidden cmd    │
└─────────┴──────┴────────────────────────┘
```

### Broadcast Features
- **Universal Addressing**: All commands use 0xFF for any sensor address
- **Auto-Discovery**: System learns actual sensor address from responses
- **Compatibility**: Works with sensors at any address (0x01, 0x02, etc.)
- **Robust Communication**: No need to know specific sensor address beforehand

## 📄 License
This project is open source. See [LICENSE](LICENSE) for details.

## 🤝 Contributing
Contributions welcome! Please read [CHANGELOG.md](CHANGELOG.md) for recent updates.

---
*Built with ❤️ for industrial IoT monitoring*

## Tính năng chính
- ✅ Đọc cảm biến SHT31/35 từ địa chỉ I2C 0x44 và 0x45
- ✅ Đọc cảm biến nhiên liệu công nghiệp qua giao thức AoooG
- ✅ Hiển thị dữ liệu trên màn hình OLED 0.91" (128x32) với 5 trang chi tiết
- ✅ Giao thức broadcast với địa chỉ 0xFF cho khả năng tương thích universal
- ✅ Hiển thị tần số thời gian thực từ cảm biến nhiên liệu
- ✅ Bộ lệnh hoàn chỉnh: đọc dữ liệu, giới hạn, firmware, số serial, calibration
- ✅ Kiểm tra tổng CRC-8/MAXIM cho tính toàn vẹn dữ liệu
- ✅ Tự động phát hiện và học địa chỉ cảm biến
- ✅ 2 LED trạng thái (hệ thống và cảm biến)
- ✅ Còi báo với các âm thanh khác nhau
- ✅ Xử lý lỗi nâng cao và thông báo trạng thái
- ✅ Cảnh báo nhiệt độ cao (>35°C)
- ✅ Interface người dùng với encoder xoay và menu điều hướng
- ✅ Chức năng factory reset và restart cảm biến

## Kết nối phần cứng

### ESP32-C3 DevKit M-1
- **SDA**: GPIO 6
- **SCL**: GPIO 5  
- **Encoder SW**: GPIO 8
- **Encoder DT**: GPIO 9
- **Encoder CLK**: GPIO 10
- **Fuel TX**: GPIO 1
- **Fuel RX**: GPIO 2
- **LED1**: GPIO 20 (LED trạng thái hệ thống)
- **LED2**: GPIO 21 (LED trạng thái cảm biến)
- **BUZZER**: GPIO 7 (Còi báo)
- **VCC**: 3.3V
- **GND**: GND

### Cảm biến nhiên liệu (AoooG Protocol)
- **Giao thức**: AoooG RS232 với broadcast addressing
- **Interface**: UART 9600 baud, 8N1
- **TX**: GPIO 1 → RX của cảm biến  
- **RX**: GPIO 2 → TX của cảm biến
- **Địa chỉ**: 0xFF (broadcast) - tương thích với mọi địa chỉ cảm biến
- **Dữ liệu**: Nhiệt độ, mức nhiên liệu, tần số, giới hạn min/max

### Cảm biến SHT31/35
- **VCC** → 3.3V
- **GND** → GND  
- **SDA** → GPIO 8
- **SCL** → GPIO 9
- **Địa chỉ I2C**: 0x44 hoặc 0x45

### Màn hình OLED 0.91"
- **VCC** → 3.3V
- **GND** → GND
- **SDA** → GPIO 8
- **SCL** → GPIO 9
- **Địa chỉ I2C**: 0x3C (mặc định)

## Cấu trúc project
```
tool fuel c3/
├── src/
│   └── main.cpp              # Code chính với logic điều khiển encoder và menu
├── lib/
│   ├── SHTSensor/            # Thư viện xử lý cảm biến SHT
│   │   ├── SHTSensor.h
│   │   └── SHTSensor.cpp
│   ├── FuelSensor/           # Thư viện xử lý cảm biến nhiên liệu AoooG
│   │   ├── FuelSensor.h
│   │   └── FuelSensor.cpp
│   ├── DisplayManager/       # Thư viện xử lý màn hình OLED với 5 trang chi tiết
│   │   ├── DisplayManager.h
│   │   └── DisplayManager.cpp
│   ├── RotaryEncoder/        # Thư viện xử lý encoder xoay với anti-jitter
│   │   ├── RotaryEncoder.h
│   │   └── RotaryEncoder.cpp
│   └── BuzzerManager/        # Thư viện xử lý buzzer PWM
│       ├── BuzzerManager.h
│       └── BuzzerManager.cpp
├── platformio.ini            # Cấu hình project
├── CHANGELOG.md              # Lịch sử thay đổi
└── README.md                 # File này
```

## Thư viện sử dụng
- `Adafruit SSD1306` - Điều khiển màn hình OLED
- `Adafruit GFX Library` - Thư viện đồ họa
- `Adafruit BusIO` - Giao tiếp I2C/SPI

## Cách sử dụng

### 1. Build project
```bash
pio run
```

### 2. Upload code
```bash
pio run --target upload
```

### 3. Monitor Serial
```bash
pio device monitor
```

## Hiển thị màn hình

### Màn hình khởi động
```
Tool Fuel C3
Initializing...
SHT + Fuel Sensors
```

### Màn hình chính (128x32)
```
┌─── DSS TOOL ─────────────┐
├─────────┬─────────────────┤
│  FUEL   │      SHT        │
│  150L   │     25.4°C      │
│         │     60%         │
└─────────┴─────────────────┘
```

### Chi tiết cảm biến nhiên liệu FUEL [1/5]
```
┌─── FUEL [1/5] ───────────┐
├─────────────────────────┤
│ T:25.0C U:150 F:59303   │ ← Nhiệt độ + Đơn vị + Tần số
│ Max:1000 Min:0          │ ← Giới hạn Max và Min
└─────────────────────────┘
```

### 5 trang chi tiết cảm biến nhiên liệu:
- **[1/5] Basic Info**: Nhiệt độ, đơn vị, tần số, giới hạn max/min
- **[2/5] Raw Data**: Dữ liệu serial thô từ cảm biến  
- **[3/5] Firmware**: Phiên bản firmware và dữ liệu hex
- **[4/5] Serial Number**: Số serial thiết bị và biểu diễn hex
- **[5/5] Status**: Thông tin giao thức, trạng thái kết nối, tính % mức nhiên liệu

### Hiển thị lỗi
```
ERROR:
No SHT sensors found!
```

## Serial Monitor Output
```
Tool Fuel C3 - Advanced Sensor Monitor
Initializing...
SHT sensor found at 0x44
SHT sensor found at 0x45
Fuel sensor initialized with broadcast address 0xFF
Found 2 SHT sensor(s) + 1 Fuel sensor
Sensor 1 (0x44): 25.4°C, 60%
Sensor 2 (0x45): 26.1°C, 58%
Fuel: 25.0°C, 150 units, 59303 Hz, Max:1000, Min:0
Response from sensor address: 0x01
Updated sensor address to: 0x01
---
```

## Giao thức AoooG - Bộ lệnh đầy đủ

### Cấu trúc lệnh
```
Yêu cầu:  31 [ADDR] [CMD] [CRC]
Phản hồi: 3E [ADDR] [CMD] [DATA...] [CRC]
```

### Các lệnh được hỗ trợ
```
┌─────────────────┬──────┬────────────────────────┐
│ Lệnh            │ Mã   │ Mô tả                  │
├─────────────────┼──────┼────────────────────────┤
│ Đọc dữ liệu     │ 06   │ Đọc nhiệt độ, mức      │
│ Đọc giới hạn    │ 07   │ Đọc min/max            │
│ Đặt mức đầy     │ 46   │ Calibration mức đầy    │
│ Đặt mức trống   │ 45   │ Calibration mức trống  │
│ Đọc tần số trống│ 51   │ Đọc tần số mức trống   │
│ Đọc firmware    │ 1C   │ Phiên bản firmware     │
│ Đọc số serial   │ 02   │ Số serial thiết bị     │
│ Factory reset   │ 18   │ Khôi phục cài đặt gốc  │
│ Khởi động lại   │ 4D   │ Restart cảm biến       │
│ Lệnh mở rộng    │ E3   │ Lệnh ẩn/mở rộng        │
└─────────────────┴──────┴────────────────────────┘
```

### Đặc điểm Broadcast
- **Địa chỉ Universal**: Tất cả lệnh dùng 0xFF cho mọi địa chỉ cảm biến
- **Tự động học**: Hệ thống học địa chỉ thực từ phản hồi cảm biến  
- **Tương thích**: Hoạt động với cảm biến ở bất kỳ địa chỉ nào (0x01, 0x02...)
- **Giao tiếp mạnh mẽ**: Không cần biết địa chỉ cụ thể của cảm biến

## Hoạt động LED và Buzzer

### LED Trạng thái
- **LED1 (GPIO 20)**: LED trạng thái hệ thống
  - Nháy mỗi giây khi hệ thống hoạt động bình thường
  - Nháy nhanh khi có lỗi khởi tạo display
  
- **LED2 (GPIO 21)**: LED trạng thái cảm biến  
  - Sáng khi có ít nhất 1 cảm biến kết nối
  - Nháy theo chu kỳ đọc dữ liệu thành công

### Buzzer PWM 8530 (GPIO 7)
- **Khởi động**: Melody tăng dần (1kHz → 1.5kHz → 2kHz)
- **Phát hiện cảm biến**: 
  - Sensor 1: 1.8kHz, 100ms
  - Sensor 2: 2.2kHz, 100ms
- **Thành công**: 2.5kHz, 300ms khi hoàn tất khởi tạo
- **Lỗi**: Melody giảm dần (800Hz → 600Hz → 400Hz)
- **Cảnh báo nhiệt độ**: Double beep 3kHz khi nhiệt độ > 35°C
- **Đọc lỗi**: 500Hz, 200ms

## Sử dụng thư viện BuzzerManager

### Khởi tạo
```cpp
#include "BuzzerManager.h"
BuzzerManager buzzer(BUZZER_PIN, 0);  // Pin 7, PWM channel 0

void setup() {
    buzzer.begin();  // Khởi tạo buzzer
}
```

### Các hàm có sẵn
```cpp
// Điều khiển cơ bản
buzzer.on();                    // Bật buzzer
buzzer.off();                   // Tắt buzzer
buzzer.beep(100);              // Beep 100ms
buzzer.beepTone(2000, 200);    // Beep 2kHz trong 200ms

// Âm thanh định sẵn
buzzer.playStartupSequence();   // Melody khởi động
buzzer.playSensorFound(1);      // Âm báo tìm thấy sensor 1
buzzer.playSensorFound(2);      // Âm báo tìm thấy sensor 2
buzzer.playSuccess();           // Âm báo thành công
buzzer.playError();             // Âm báo lỗi
buzzer.playWarning();           // Âm báo cảnh báo
buzzer.playTemperatureAlert();  // Âm báo nhiệt độ cao
buzzer.playReadError();         // Âm báo lỗi đọc

// Cấu hình
buzzer.setDefaultFrequency(2700);  // Đặt tần số mặc định
buzzer.setDutyCycle(128);          // Đặt duty cycle (0-255)
```

## Tuỳ chỉnh

### Thay đổi chân LED và Buzzer
```cpp
#define LED1_PIN 20      // LED trạng thái hệ thống
#define LED2_PIN 21      // LED trạng thái cảm biến  
#define BUZZER_PIN 7     // Còi PWM 8530
```

### Tuỳ chỉnh âm thanh buzzer 8530
```cpp
// Thay đổi tần số chuẩn
const int BUZZER_FREQ = 2700;  // 2.7kHz cho 8530

// Sử dụng tần số tùy chỉnh
beepTone(1500, 200);  // 1.5kHz trong 200ms

// Thay đổi duty cycle (âm lượng)
const int BUZZER_DUTY = 128;  // 50% (0-255)
```

### Thay đổi ngưỡng cảnh báo nhiệt độ
```cpp
if ((temp1 > 35.0 && !isnan(temp1)) || (temp2 > 35.0 && !isnan(temp2))) {
    beepTone(3000, 100); // Thay đổi 35.0 và tần số cảnh báo
    delay(100);
    beepTone(3000, 100); // Double beep
}
```

## Đặc điểm Buzzer 8530
- **Loại**: Magnetic buzzer PWM
- **Điện áp hoạt động**: 3-5V DC  
- **Tần số tối ưu**: 2.7kHz (2700Hz)
- **Dòng tiêu thụ**: ~30mA
- **Âm lượng**: 85dB tại 10cm
- **Điều khiển**: PWM với duty cycle 50%

### Thay đổi chân I2C
Sửa trong `main.cpp`:
```cpp
#define SDA_PIN 6
#define SCL_PIN 5
```

### Thay đổi tần suất đọc
Sửa trong `main.cpp`:
```cpp
const unsigned long READ_INTERVAL = 2000; // milliseconds
```

### Thay đổi địa chỉ OLED
Sửa trong `main.cpp`:
```cpp
DisplayManager display(128, 32, 0x3C);  // 0x3C hoặc 0x3D
```

## Troubleshooting

### 1. Màn hình OLED không sáng
- Kiểm tra kết nối I2C (GPIO 6 SDA, GPIO 5 SCL)
- Thử địa chỉ 0x3D thay vì 0x3C
- Kiểm tra nguồn 3.3V

### 2. Không đọc được cảm biến SHT
- Kiểm tra kết nối I2C
- Kiểm tra địa chỉ cảm biến (0x44 hoặc 0x45)
- Kiểm tra nguồn 3.3V

### 3. Cảm biến nhiên liệu không phản hồi
- Kiểm tra kết nối UART (GPIO 1 TX, GPIO 2 RX)
- Xác nhận baud rate 9600, 8N1
- Kiểm tra nguồn và dây mass
- Sử dụng Serial Monitor để xem debug messages

### 4. Encoder không hoạt động
- Kiểm tra kết nối GPIO 8 (SW), GPIO 9 (DT), GPIO 10 (CLK)
- Kiểm tra pull-up resistor nội hoặc ngoại
- Xem Serial Monitor để debug encoder events

### 5. Build failed
- Kiểm tra thư viện đã được cài đặt đúng
- Xóa folder `.pio` và build lại
- Cập nhật PlatformIO lên phiên bản mới nhất

## Phát triển thêm
- [ ] WiFi connectivity cho remote monitoring
- [ ] Lưu dữ liệu vào SPIFFS với timestamp  
- [ ] Web interface cho configuration
- [ ] Cảnh báo ngưỡng qua email/SMS
- [ ] Bluetooth Low Energy cho mobile app
- [ ] Data logging với múi giờ
- [ ] Calibration wizard với guided setup
- [ ] Multi-sensor network support