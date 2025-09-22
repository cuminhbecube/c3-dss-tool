# Scrollable Detail View Feature

## 📋 **Mô tả tính năng**
Tính năng cuộn (scroll) cho detail view cho phép xem nhiều thông tin hơn trên màn hình OLED 128x32 hạn chế.

## 🎮 **Cách sử dụng**

### **Điều hướng cơ bản:**
1. **Main Menu** → Rotate encoder để chọn FUEL hoặc SHT
2. **Single Click** → Vào detail view (scroll position = 0)
3. **Trong Detail View:**
   - **Rotate Encoder** → Cuộn qua các trang thông tin
   - **Single Click** → Quay về main menu

### **Fuel Detail View - 3 trang:**

#### **Trang 1/3: Basic Info**
```
FUEL [1/3]
------------
Temperature: 25.0C
Level: 100L
Max: 200L
Min: 10L
Rotate: Scroll
```

#### **Trang 2/3: Raw Data**
```
FUEL [2/3]
------------
Raw Serial Data:
3E 01 06 25 64 00
A5
Rotate: Scroll
```

#### **Trang 3/3: Additional Info**
```
FUEL [3/3]
------------
Sensor Status:
Protocol: AoooG
Baud: 9600
Connection: RS232
Percent: 45.0%
```

### **SHT Detail View - 3 trang:**

#### **Trang 1/3: Large Display**
```
SHT [1/3]
------------
25.0C    60%

```

#### **Trang 2/3: Detailed Info**
```
SHT [2/3]
------------
Temp: 25.02C
Status: Normal
Humidity: 60.5%
Level: Normal
```

#### **Trang 3/3: Sensor Info**
```
SHT [3/3]
------------
SHT Sensor Info:
Address: 0x44
Protocol: I2C
Type: SHT3x
Connection: OK
```

## 🔧 **Implementation Details**

### **Biến mới:**
```cpp
int detailScrollPosition = 0;           // Vị trí scroll hiện tại
const int MAX_SCROLL_POSITIONS = 3;     // Số trang tối đa
```

### **Encoder Handling:**
```cpp
case MENU_FUEL_DETAIL:
case MENU_SHT_DETAIL:
    // Scroll through detail view sections
    detailScrollPosition = (detailScrollPosition + positionChange + MAX_SCROLL_POSITIONS) % MAX_SCROLL_POSITIONS;
    break;
```

### **Auto Reset:**
- Scroll position được reset về 0 khi:
  - Vào detail view từ main menu
  - Thoát detail view về main menu
  - Chuyển giữa các menu khác

## ✨ **Lợi ích**

### **1. Hiển thị nhiều thông tin hơn**
- Vượt qua giới hạn 4 dòng của OLED 128x32
- Tổ chức thông tin theo nhóm logic

### **2. User Experience tốt hơn**
- Navigation trực quan với indicator [x/3]
- Hint "Rotate: Scroll" ở cuối màn hình
- Smooth scrolling với encoder

### **3. Flexible Data Display**
- Raw data có thể rất dài → riêng 1 trang
- Status info không làm cluttered main view
- Percentage calculation chỉ hiện khi cần

## 🎯 **Navigation Flow**

```
Main Menu
    ↓ (Single Click on FUEL)
Fuel Detail [1/3] - Basic Info
    ↓ (Rotate Right)
Fuel Detail [2/3] - Raw Data  
    ↓ (Rotate Right)
Fuel Detail [3/3] - Additional Info
    ↓ (Rotate Right)
Fuel Detail [1/3] - Basic Info (Loop)
    ↓ (Single Click)
Main Menu
```

## 🔍 **Debug Features**
- Serial output cho scroll position: `"Detail scroll position: 2"`
- Scroll position được reset log: `"Entering fuel detail mode"`
- Encoder change detection: Immediate visual feedback

## 📱 **Display Constraints**
- **Screen**: 128x32 pixels
- **Font size 1**: ~21 characters per line, 4 lines max
- **Auto line wrap**: Long hex strings tự động xuống dòng
- **Bottom hint**: "Rotate: Scroll" fixed position

## 🚀 **Extensibility**
- Dễ dàng thêm trang mới bằng cách tăng `MAX_SCROLL_POSITIONS`
- Switch case structure cho phép thêm content mới
- Có thể customize cho từng sensor type khác nhau