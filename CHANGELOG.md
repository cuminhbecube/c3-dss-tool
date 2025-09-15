# Changelog - ESP32-C3 DSS Tool

## Latest Features (September 2025)

### 🚀 **Performance Optimizations**
- **Instant Display Response**: Reduced encoder lag from 0-2000ms to 10-50ms
- **Optimized Update Mechanism**: Separated display updates from sensor readings
- **Force Update Flag**: Immediate visual feedback on encoder rotation

### 🎨 **Enhanced User Interface**
- **Inverted Highlight Colors**: Selected menu items now show white background with black text
- **Two-Column Layout**: Fuel on left, SHT on right for better space utilization
- **Larger Font Sizes**: Improved readability with size 2 font for main values
- **Clean Header**: Removed "READY" text clutter, shows only "NO SEN" when needed

### 🔄 **Menu System Improvements**
- **Streamlined Navigation**: Reduced from 3 to 2 main options (FUEL, SHT)
- **Hidden Settings Access**: Double-click to enter calibration mode (no visual clutter)
- **Responsive Navigation**: Encoder changes trigger immediate display updates

### 🔌 **Hotswap Sensor Detection**
- **Automatic Detection**: Sensors detected within 5 seconds when plugged in
- **Real-time Monitoring**: Continuous monitoring for sensor connect/disconnect
- **Visual Notifications**: Display alerts when sensors are added/removed
- **Audio Feedback**: Buzzer confirms sensor connections

### 🛠 **Technical Enhancements**
- **Enhanced FuelSensor**: Added `isConnected()` method for connectivity checks
- **DisplayManager Improvements**: Added inverted color methods and notifications
- **Optimized Main Loop**: Separated sensor reading from display updates
- **Better Error Handling**: Improved sensor communication and error recovery

### 📱 **Display Layout**
```
┌─────────┬─────────┐
│  FUEL   │   SHT   │
│   50L   │  25°C   │
│         │  60%    │
└─────────┴─────────┘
```

### 🎛 **Control Scheme**
- **Single Click**: Navigate between FUEL ↔ SHT
- **Single Click on Selection**: Enter detail view
- **Double Click**: Enter settings/calibration mode
- **Long Press**: Execute setting commands

## Hardware Support
- **MCU**: ESP32-C3 DevKit M-1
- **Display**: SSD1306 OLED 128x32
- **Sensors**: SHT31/35 (I2C), Fuel sensor (RS232)
- **Input**: Rotary encoder with button
- **Audio**: Buzzer for feedback
- **Indicators**: Dual LED status

## Libraries Used
- Adafruit SSD1306 & GFX
- Custom sensor libraries
- Hardware interrupt handling