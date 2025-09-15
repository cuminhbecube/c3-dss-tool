# ESP32-C3 DSS Tool - Advanced Sensor Monitor

## 🚀 Overview
High-performance IoT monitoring system built on ESP32-C3 for industrial fuel and environmental sensors with optimized OLED interface and real-time hotswap detection.

## ✨ Key Features

### 🎛 **Advanced User Interface**
- **Instant Response**: 10-50ms encoder response time (40-200x faster)
- **Inverted Highlights**: Professional menu selection with color inversion  
- **Two-Column Layout**: Fuel (left) ↔ SHT (right) with large fonts
- **Clean Design**: Minimalist interface, hidden advanced settings

### 🔌 **Smart Sensor Management**
- **Hotswap Detection**: Auto-detect sensors within 5 seconds
- **Dual Sensor Support**: SHT31/35 + Industrial fuel sensor
- **Real-time Monitoring**: Continuous connectivity checking
- **Visual/Audio Feedback**: LED + buzzer notifications

### 📱 **Optimized Display**
- **128x32 OLED**: Perfect layout utilization
- **Large Fonts**: Size 2 for main values, excellent readability
- **Status Indicators**: Connection status, sensor health
- **Multi-mode Views**: Main, detail, calibration screens

### 🎮 **Intuitive Controls**
- **Single Click**: Navigate FUEL ↔ SHT
- **Click on Selection**: Enter detail view
- **Double Click**: Hidden calibration access
- **Long Press**: Execute calibration commands

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

### ⛽ **Fuel Sensor**: Industrial RS232
- **Protocol**: AoooG RS232 
- **Interface**: UART (9600 baud)
- **Data**: Temperature + fuel level
- **Calibration**: Set full/empty points

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
- **DisplayManager**: Advanced OLED graphics and layouts
- **SHTSensor**: Temperature/humidity with error handling  
- **FuelSensor**: Industrial protocol with connectivity checks
- **RotaryEncoder**: Anti-jitter with multi-click detection
- **BuzzerManager**: Audio patterns and feedback

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

### Navigation Flow
```
Main Menu → [Single Click] → Detail View
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
- ✅ Instant encoder response optimization
- ✅ Inverted color highlights  
- ✅ Two-column layout implementation
- ✅ Hotswap sensor detection
- ✅ Menu system streamlining
- ✅ Enhanced visual feedback

## 📄 License
This project is open source. See [LICENSE](LICENSE) for details.

## 🤝 Contributing
Contributions welcome! Please read [CHANGELOG.md](CHANGELOG.md) for recent updates.

---
*Built with ❤️ for industrial IoT monitoring*

## Tính năng
- ✅ Đọc cảm biến SHT31/35 từ địa chỉ I2C 0x44 và 0x45
- ✅ Hiển thị dữ liệu lên màn hình OLED 0.91" (128x32)
- ✅ 2 LED trạng thái (hệ thống và cảm biến)
- ✅ Còi báo với các âm thanh khác nhau
- ✅ Tự động phát hiện cảm biến có sẵn
- ✅ Xử lý lỗi khi cảm biến không kết nối
- ✅ Cảnh báo nhiệt độ cao (>35°C)
- ✅ Output Serial Monitor để debug
- ✅ Cập nhật dữ liệu mỗi 2 giây

## Kết nối phần cứng

### ESP32-C3 DevKit M-1
- **SDA**: GPIO 6
- **SCL**: GPIO 5
- **LED1**: GPIO 20 (LED trạng thái hệ thống)
- **LED2**: GPIO 21 (LED trạng thái cảm biến)
- **BUZZER**: GPIO 7 (Còi báo)
- **VCC**: 3.3V
- **GND**: GND

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
│   └── main.cpp              # Code chính
├── lib/
│   ├── SHTSensor/            # Thư viện xử lý cảm biến SHT
│   │   ├── SHTSensor.h
│   │   └── SHTSensor.cpp
│   ├── DisplayManager/       # Thư viện xử lý màn hình OLED
│   │   ├── DisplayManager.h
│   │   └── DisplayManager.cpp
│   └── BuzzerManager/        # Thư viện xử lý buzzer PWM
│       ├── BuzzerManager.h
│       └── BuzzerManager.cpp
├── platformio.ini            # Cấu hình project
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
SHT Sensors
```

### Hiển thị dữ liệu (màn hình nhỏ 128x32)
```
SHT Sensor Data
Temp: 25.4 C
Hum:  60 %
```

### Hiển thị lỗi
```
ERROR:
No SHT sensors found!
```

## Serial Monitor Output
```
Tool Fuel C3 - SHT Sensor Monitor
Initializing...
SHT sensor found at 0x44
SHT sensor found at 0x45
Found 2 SHT sensor(s)
Sensor 1 (0x44): 25.4°C, 60%
Sensor 2 (0x45): 26.1°C, 58%
---
```

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
- Kiểm tra kết nối I2C
- Thử địa chỉ 0x3D thay vì 0x3C
- Kiểm tra nguồn 3.3V

### 2. Không đọc được cảm biến
- Kiểm tra kết nối I2C
- Kiểm tra địa chỉ cảm biến (0x44 hoặc 0x45)
- Kiểm tra nguồn 3.3V

### 3. Build failed
- Kiểm tra thư viện đã được cài đặt đúng
- Xóa folder `.pio` và build lại

## Phát triển thêm
- [ ] Thêm WiFi connectivity
- [ ] Lưu dữ liệu vào SPIFFS
- [ ] Web interface
- [ ] Cảnh báo ngưỡng
- [ ] Bluetooth Low Energy