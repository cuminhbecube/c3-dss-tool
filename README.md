# Tool Fuel C3 - SHT ### Cảm biến### Màn hình OLED 0.91"
- **VCC** → 3.3V
- **GND** → GND
- **SDA** → GPIO 6
- **SCL** → GPIO 5
- **Địa chỉ I2C**: 0x3C (mặc định)

### LED và Buzzer
- **LED1** (GPIO 20) → LED trạng thái hệ thống + Resistor 220Ω → GND
- **LED2** (GPIO 21) → LED trạng thái cảm biến + Resistor 220Ω → GND  
- **BUZZER** (GPIO 7) → Còi 8530 PWM (3.3V, tần số 2.7kHz)/35
- **VCC** → 3.3V
- **GND** → GND  
- **SDA** → GPIO 6
- **SCL** → GPIO 5
- **Địa chỉ I2C**: 0x44 hoặc 0x45 Monitor

## Mô tả
Project này sử dụng ESP32-C3 để đọc dữ liệu từ cảm biến nhiệt độ và độ ẩm SHT31/35 và hiển thị lên màn hình OLED 0.91".

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