# Test Raw Data Display Feature

## Mô tả tính năng
Khi vào detail view của fuel sensor (nhấn chọn trên FUEL trong main menu), màn hình OLED sẽ hiển thị:

1. **Thông tin cơ bản**: Nhiệt độ, mức nhiên liệu
2. **Raw data từ Serial 1**: Dữ liệu hex thô nhận được từ cảm biến

## Cách hoạt động

### 1. Lưu trữ Raw Data
- `FuelSensor` lưu trữ response cuối cùng trong `lastRawResponse[]`
- Tạo chuỗi hex formatted trong `lastRawDataString`
- Format: "3E 01 06 25 64 00 A5" (ví dụ)

### 2. Hiển thị trên OLED
```
FUEL + RAW DATA
---------------
T:25.0C L:100L
Raw Serial Data:
3E 01 06 25 64 00
A5
```

### 3. Cách sử dụng
1. Từ main menu, rotate encoder để chọn "FUEL"
2. Single click để vào detail view
3. Màn hình sẽ hiển thị dữ liệu fuel + raw data
4. Single click lại để quay về main menu

## Expected Output trên Serial Monitor

```
Fuel Sensor (RS232): 25.0°C, 100L
Raw Data: 3E 01 06 25 64 00 A5
```

## Format Raw Data
- Mỗi byte được hiển thị dưới dạng 2 ký tự hex (uppercase)
- Các byte cách nhau bởi dấu cách
- Tự động xuống dòng nếu quá dài (21 ký tự/dòng)

## Debugging
- Raw data được in ra Serial Monitor mỗi khi đọc dữ liệu
- Nếu không có raw data, sẽ hiển thị fuel details thông thường
- Check `fuelSensor.getLastRawData()` để xem có dữ liệu không

## Test Cases

### Case 1: Normal Response
```
Request:  31 FF 06 29
Response: 3E 01 06 25 64 00 A5
Display:  "3E 01 06 25 64 00 A5"
```

### Case 2: Long Response
```
Response: 3E 01 06 25 64 00 12 34 56 78 9A BC DE F0
Display:  Line 1: "3E 01 06 25 64 00 12"
          Line 2: "34 56 78 9A BC DE F0"
```

### Case 3: No Response
```
Display: Standard fuel details without raw data
```

## Lợi ích
1. **Debugging**: Xem được dữ liệu thô từ sensor
2. **Protocol Analysis**: Phân tích giao thức communication
3. **Troubleshooting**: Phát hiện lỗi trong quá trình truyền dữ liệu
4. **Development**: Hỗ trợ phát triển và test protocol mới