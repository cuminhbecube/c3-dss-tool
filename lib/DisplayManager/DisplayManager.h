#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

class DisplayManager {
public:
    DisplayManager(uint8_t width = 128, uint8_t height = 32, uint8_t address = 0x3C);
    bool begin(int sda_pin = -1, int scl_pin = -1);
    void clear();
    void display();
    
    // Display sensor data
    void showSensorData(float temp1, float hum1, float temp2 = NAN, float hum2 = NAN, bool sensor2Available = false);
    void showSensorAndFuelData(float shtTemp, float shtHum, float fuelTemp, int fuelLevel);
    void showSensorAndFuelWithLimits(float shtTemp, float shtHum, float fuelTemp, int fuelLevel, 
                                   int levelMax, int levelMin);
    void showFuelDetails(float fuelTemp, int fuelLevel, int levelMax, int levelMin);
    void showFuelDetailsWithRaw(float fuelTemp, int fuelLevel, int levelMax, int levelMin, const String& rawData);
    void showFuelDetailsScrollable(float fuelTemp, int fuelLevel, int levelMax, int levelMin, uint16_t frequency, const String& rawData, 
                                 const uint8_t* firmwareData, int firmwareLen, 
                                 const uint8_t* serialData, int serialLen, uint32_t serialNumber, int scrollPos);
    void showSHTDetails(float shtTemp, float shtHum, uint8_t address);
    void showSHTDetailsScrollable(float shtTemp, float shtHum, uint8_t address, int scrollPos);
    void showSHTLargeDisplay(float shtTemp, float shtHum);
    void showSystemInfo(bool sht_available, bool fuel_available, uint8_t sht_address, int displayMode, int timeoutCounter);
    void showMainMenu(float shtTemp, float shtHum, float fuelTemp, int fuelLevel, int highlight, bool sht_available, bool fuel_available);
    void showSettingMenu(int currentSetting);
    void showSettingMenuWithProgress(int currentSetting, int progressPercent);
    void showExtendedMenu(int currentExtended);
    void showExtendedResults(const uint8_t* firmwareData, int firmwareLen, 
                           const uint8_t* extendedData, int extendedLen);
    void showDSSTool();
    void showStartupMessage();
    void showError(const char* message);
    void showConnecting();
    void showNotification(const char* title, const char* message);
    
    // Helper methods for direct access
    void setCursor(int x, int y);
    void print(const char* text);
    void printInverted(int x, int y, const char* text);
    void fillInvertedRect(int x, int y, int w, int h);
    
private:
    Adafruit_SSD1306* _display;
    uint8_t _width;
    uint8_t _height;
    uint8_t _address;
    
    void drawSensorBox(int x, int y, int w, int h, const char* title, float temp, float hum, bool valid);
};

#endif // DISPLAYMANAGER_H