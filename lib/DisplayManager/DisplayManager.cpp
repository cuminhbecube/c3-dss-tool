#include "DisplayManager.h"

DisplayManager::DisplayManager(uint8_t width, uint8_t height, uint8_t address) {
    _width = width;
    _height = height;
    _address = address;
    _display = new Adafruit_SSD1306(width, height, &Wire, -1);
}

bool DisplayManager::begin(int sda_pin, int scl_pin) {
    if (sda_pin != -1 && scl_pin != -1) {
        Wire.begin(sda_pin, scl_pin);
    } else {
        Wire.begin();
    }
    
    if (!_display->begin(SSD1306_SWITCHCAPVCC, _address)) {
        return false;
    }
    
    _display->clearDisplay();
    _display->setTextColor(SSD1306_WHITE);
    _display->setTextSize(1);
    _display->display();
    
    return true;
}

void DisplayManager::clear() {
    _display->clearDisplay();
}

void DisplayManager::display() {
    _display->display();
}

void DisplayManager::showStartupMessage() {
    clear();
    _display->setTextSize(1);
    _display->setCursor(0, 0);
    _display->println("Tool Fuel C3");
    _display->println("Initializing...");
    _display->println("SHT Sensors");
    display();
}

void DisplayManager::showError(const char* message) {
    clear();
    _display->setTextSize(1);
    _display->setCursor(0, 0);
    _display->println("ERROR:");
    _display->println(message);
    display();
}

void DisplayManager::showConnecting() {
    clear();
    _display->setTextSize(1);
    _display->setCursor(0, 0);
    _display->println("Connecting to");
    _display->println("SHT sensors...");
    display();
}

void DisplayManager::drawSensorBox(int x, int y, int w, int h, const char* title, float temp, float hum, bool valid) {
    _display->drawRect(x, y, w, h, SSD1306_WHITE);
    
    _display->setTextSize(1);
    _display->setCursor(x + 2, y + 1);
    _display->print(title);
    
    if (valid) {
        _display->setCursor(x + 2, y + 10);
        _display->print(temp, 1);
        _display->print("C");
        
        _display->setCursor(x + 2, y + 19);
        _display->print(hum, 0);
        _display->print("%");
    } else {
        _display->setCursor(x + 2, y + 12);
        _display->print("N/A");
    }
}

void DisplayManager::showSensorData(float temp1, float hum1, float temp2, float hum2, bool sensor2Available) {
    clear();
    
    if (_height >= 64) {
        // Large display - show both sensors side by side
        drawSensorBox(0, 0, 63, 30, "SHT1", temp1, hum1, !isnan(temp1));
        if (sensor2Available) {
            drawSensorBox(65, 0, 63, 30, "SHT2", temp2, hum2, !isnan(temp2));
        }
        
        // Status line
        _display->setCursor(0, 54);
        _display->setTextSize(1);
        _display->print("Status: ");
        if (!isnan(temp1) || (sensor2Available && !isnan(temp2))) {
            _display->print("OK");
        } else {
            _display->print("ERROR");
        }
    } else {
        // Small display - show primary sensor only
        _display->setTextSize(1);
        _display->setCursor(0, 0);
        _display->print("SHT Sensor Data");
        
        _display->setCursor(0, 10);
        if (!isnan(temp1)) {
            _display->print("Temp: ");
            _display->print(temp1, 1);
            _display->print(" C");
        } else {
            _display->print("Temp: N/A");
        }
        
        _display->setCursor(0, 20);
        if (!isnan(hum1)) {
            _display->print("Hum:  ");
            _display->print(hum1, 0);
            _display->print(" %");
        } else {
            _display->print("Hum:  N/A");
        }
    }
    
    display();
}

void DisplayManager::showSensorAndFuelData(float shtTemp, float shtHum, 
                                          float fuelTemp, int fuelLevel) {
    clear();
    
    // SHT sensor data (line 1-2)
    _display->setCursor(0, 0);
    if (!isnan(shtTemp)) {
        _display->print("SHT: ");
        _display->print(shtTemp, 1);
        _display->print("C ");
        _display->print(shtHum, 0);
        _display->print("%");
    } else {
        _display->print("SHT: N/A");
    }
    
    // Fuel sensor data (line 3-4)
    _display->setCursor(0, 15);
    if (!isnan(fuelTemp)) {
        _display->print("Fuel: ");
        _display->print(fuelTemp, 1);
        _display->print("C");
    } else {
        _display->print("Fuel: N/A");
    }
    
    _display->setCursor(0, 25);
    if (fuelLevel >= 0) {
        _display->print("Level: ");
        _display->print(fuelLevel);
        _display->print(" L");
    } else {
        _display->print("Level: N/A");
    }
    
    display();
}

void DisplayManager::showSensorAndFuelWithLimits(float shtTemp, float shtHum, 
                                                float fuelTemp, int fuelLevel,
                                                int levelMax, int levelMin) {
    clear();
    
    // SHT sensor data (line 1)
    _display->setCursor(0, 0);
    if (!isnan(shtTemp)) {
        _display->print("T:");
        _display->print(shtTemp, 1);
        _display->print("C H:");
        _display->print(shtHum, 0);
        _display->print("%");
    } else {
        _display->print("SHT: N/A");
    }
    
    // Fuel temperature (line 2)
    _display->setCursor(0, 8);
    if (!isnan(fuelTemp)) {
        _display->print("Fuel: ");
        _display->print(fuelTemp, 1);
        _display->print("C");
    } else {
        _display->print("Fuel: N/A");
    }
    
    // Current level (line 3)
    _display->setCursor(0, 16);
    if (fuelLevel >= 0) {
        _display->print("Lvl: ");
        _display->print(fuelLevel);
        _display->print("L");
    } else {
        _display->print("Lvl: N/A");
    }
    
    // Max/Min levels (line 4)
    _display->setCursor(0, 24);
    if (levelMax > 0 && levelMin >= 0) {
        _display->print("Max:");
        _display->print(levelMax);
        _display->print(" Min:");
        _display->print(levelMin);
    } else {
        _display->print("Limits: N/A");
    }
    
    display();
}

void DisplayManager::showFuelDetails(float fuelTemp, int fuelLevel, int levelMax, int levelMin) {
    clear();
    
    // Title with fuel icon
    _display->setCursor(0, 0);
    _display->print("FUEL SENSOR DETAIL");
    
    // Draw horizontal separator
    _display->drawLine(0, 7, 128, 7, WHITE);
    
    // Temperature with thermometer icon
    _display->setCursor(0, 12);
    _display->print("TEMP:");
    _display->setCursor(50, 12);
    if (!isnan(fuelTemp)) {
        _display->print(fuelTemp, 1);
        _display->print("C");
        
        // Simple thermometer visualization
        if (fuelTemp > 70) {
            _display->print(" HOT!");
        } else if (fuelTemp < 10) {
            _display->print(" COLD");
        }
    } else {
        _display->print("N/A");
    }
    
    // Current level with tank visualization
    _display->setCursor(0, 20);
    _display->print("LEVEL:");
    _display->setCursor(50, 20);
    if (fuelLevel >= 0) {
        _display->print(fuelLevel);
        _display->print("L");
        
        // Tank level visualization
        if (levelMax > 0 && levelMin >= 0) {
            int percentage = map(fuelLevel, levelMin, levelMax, 0, 100);
            _display->setCursor(85, 20);
            _display->print("(");
            _display->print(percentage);
            _display->print("%)");
        }
    } else {
        _display->print("N/A");
    }
    
    // Limits info
    _display->setCursor(0, 28);
    if (levelMax > 0 && levelMin >= 0) {
        _display->print("Range: ");
        _display->print(levelMin);
        _display->print("-");
        _display->print(levelMax);
        _display->print("L");
    } else {
        _display->print("Range: Not Set");
    }
    
    display();
}

void DisplayManager::showSHTDetails(float shtTemp, float shtHum, uint8_t address) {
    clear();
    
    _display->setCursor(0, 0);
    _display->print("=== SHT SENSOR ===");
    
    _display->setCursor(0, 8);
    _display->print("Address: 0x");
    _display->print(address, HEX);
    
    _display->setCursor(0, 16);
    if (!isnan(shtTemp)) {
        _display->print("Temp: ");
        _display->print(shtTemp, 2);
        _display->print("C");
    } else {
        _display->print("Temp: N/A");
    }
    
    _display->setCursor(0, 24);
    if (!isnan(shtHum)) {
        _display->print("Humidity: ");
        _display->print(shtHum, 0);
        _display->print("%");
    } else {
        _display->print("Humidity: N/A");
    }
    
    display();
}

void DisplayManager::showSystemInfo(bool sht_available, bool fuel_available, uint8_t sht_address, int displayMode, int timeoutCounter) {
    clear();
    
    _display->setCursor(0, 0);
    _display->print("=== SYSTEM INFO ===");
    
    _display->setCursor(0, 8);
    _display->print("Mode: ");
    _display->print(displayMode + 1);
    _display->print("/4");
    
    _display->setCursor(0, 16);
    _display->print("SHT: ");
    if (sht_available) {
        _display->print("0x");
        _display->print(sht_address, HEX);
    } else {
        _display->print("N/A");
    }
    
    _display->setCursor(0, 24);
    _display->print("Fuel: ");
    _display->print(fuel_available ? "OK" : "N/A");
    
    _display->setCursor(0, 32);
    _display->print("Timeout: ");
    _display->print(timeoutCounter);
    _display->print("s");
    
    display();
}

void DisplayManager::showMainMenu(float shtTemp, float shtHum, float fuelTemp, int fuelLevel, 
                                 int highlight, bool sht_available, bool fuel_available) {
    clear();

    _display->setCursor(25, 0);
    _display->print("== DSS Tool ==");
    
    // Draw horizontal line under title
    _display->drawLine(0, 8, 128, 8, WHITE);
    
    // Set larger text size for better visibility
    _display->setTextSize(1);
    
    // Left side - FUEL section (0-63 pixels)
    if (highlight == 0) { // HIGHLIGHT_FUEL
        // Draw inverted background for highlighted fuel section
        _display->fillRect(0, 8, 64, 24, WHITE);
        _display->setTextColor(BLACK);
        
        // Fuel title
        _display->setCursor(2, 10);
        _display->print("FUEL");
        
        // Fuel value with larger text
        _display->setTextSize(2);
        _display->setCursor(2, 18);
        if (fuel_available && fuelLevel >= 0) {
            _display->print(fuelLevel);
            _display->setTextSize(1);
            _display->print("L");
        } else {
            _display->setTextSize(1);
            _display->print("N/A");
        }
        _display->setTextColor(WHITE); // Reset color
    } else {
        // Normal fuel display
        _display->setCursor(2, 10);
        _display->print("FUEL");
        
        _display->setTextSize(2);
        _display->setCursor(2, 18);
        if (fuel_available && fuelLevel >= 0) {
            _display->print(fuelLevel);
            _display->setTextSize(1);
            _display->print("L");
        } else {
            _display->setTextSize(1);
            _display->print("N/A");
        }
    }
    
    // Right side - SHT section (64-127 pixels)
    if (highlight == 1) { // HIGHLIGHT_SHT
        // Draw inverted background for highlighted SHT section
        _display->fillRect(64, 8, 64, 24, WHITE);
        _display->setTextColor(BLACK);
        
        // SHT title
        _display->setTextSize(1);
        _display->setCursor(66, 10);
        _display->print("SHT");
        
        // Temperature and humidity with larger text
        if (sht_available && !isnan(shtTemp) && !isnan(shtHum)) {
            _display->setCursor(66, 18);
            _display->print((int)shtTemp);
            _display->print("C");
            _display->setCursor(66, 26);
            _display->print((int)shtHum);
            _display->print("%");
        } else {
            _display->setCursor(66, 22);
            _display->print("N/A");
        }
        _display->setTextColor(WHITE); // Reset color
    } else {
        // Normal SHT display
        _display->setTextSize(1);
        _display->setCursor(66, 10);
        _display->print("SHT");
        
        if (sht_available && !isnan(shtTemp) && !isnan(shtHum)) {
            _display->setCursor(66, 18);
            _display->print((int)shtTemp);
            _display->print("C");
            _display->setCursor(66, 26);
            _display->print((int)shtHum);
            _display->print("%");
        } else {
            _display->setCursor(66, 22);
            _display->print("N/A");
        }
    }
    
    // Draw vertical separator line
    _display->drawLine(63, 8, 63, 31, WHITE);
    
    // Reset text size to normal
    _display->setTextSize(1);
    
    display();
}

void DisplayManager::showSettingMenu(int currentSetting) {
    clear();
    
    // Title with border
    _display->setCursor(14, 0);
    _display->print("== CALIBRATION ==");
    
    // Draw horizontal line under title
    _display->drawLine(0, 7, 128, 7, WHITE);
    
    // Set Full option with icon
    if (currentSetting == 0) { // SETTING_SET_FULL
        // Draw inverted background for highlighted item
        _display->fillRect(0, 9, 128, 8, WHITE);
        _display->setTextColor(BLACK);
        _display->setCursor(0, 9);
        _display->print(">");
        _display->fillRect(10, 10, 6, 6, BLACK); // Filled square icon
        _display->setCursor(20, 9);
        _display->print("Set FULL Tank");
        _display->setTextColor(WHITE); // Reset color
    } else {
        _display->setCursor(0, 9);
        _display->print(" ");
        _display->drawRect(10, 10, 6, 6, WHITE); // Empty square icon
        _display->setCursor(20, 9);
        _display->print("Set FULL Tank");
    }
    
    // Set Empty option with icon
    if (currentSetting == 1) { // SETTING_SET_EMPTY
        // Draw inverted background for highlighted item
        _display->fillRect(0, 17, 128, 8, WHITE);
        _display->setTextColor(BLACK);
        _display->setCursor(0, 17);
        _display->print(">");
        _display->fillRect(10,18, 6, 6, BLACK); // Filled square icon
        _display->setCursor(20, 17);
        _display->print("Set EMPTY Tank");
        _display->setTextColor(WHITE); // Reset color
    } else {
        _display->setCursor(0, 17);
        _display->print(" ");
        _display->drawRect(10, 18, 6, 6, WHITE); // Empty square icon
        _display->setCursor(20, 17);
        _display->print("Set EMPTY Tank");
    }
    
    // Instructions at bottom
    _display->setCursor(0, 25);
    _display->print("Hold 3s to confirm");
    
    display();
}

void DisplayManager::showDSSTool() {
    clear();
    
    // Draw border frame
    _display->drawRect(0, 0, 128, 32, WHITE);
    _display->drawRect(1, 1, 126, 30, WHITE);
    
    // Main title centered
    _display->setCursor(35, 8);
    _display->print("DSS TOOL");
    
    // Subtitle
    _display->setCursor(25, 18);
    _display->print("No Sensors Found");
    
    // Add some decorative elements
    _display->fillCircle(15, 16, 2, WHITE);
    _display->fillCircle(113, 16, 2, WHITE);
    
    display();
}

void DisplayManager::showSHTLargeDisplay(float shtTemp, float shtHum) {
    clear();
    
    if (isnan(shtTemp) || isnan(shtHum)) {
        // Show error message if no data
        _display->setCursor(20, 8);
        _display->print("SHT SENSOR");
        _display->setCursor(30, 20);
        _display->print("NO DATA");
        display();
        return;
    }
    
    // Set large text size
    _display->setTextSize(2);
    
    // Temperature display - top half
    _display->setCursor(5, 2);
    _display->print(shtTemp, 2);
    _display->print(" C");
    
    // Small degree symbol manually (since large font doesn't show it well)
    _display->setTextSize(1);
    _display->setCursor(69, 2);
    _display->print("o");
    
    // Humidity display - bottom half
    _display->setTextSize(2);
    _display->setCursor(5, 18);
    _display->print(shtHum, 0);
    _display->print("%");
    
    // Add small labels
    _display->setTextSize(1);
    _display->setCursor(100, 8);
    _display->print("TEMP");
    _display->setCursor(100, 24);
    _display->print("HUM");
    
    // Add separator line
    _display->drawLine(0, 16, 128, 16, WHITE);
    
    // Reset text size to normal
    _display->setTextSize(1);
    
    display();
}

void DisplayManager::showNotification(const char* title, const char* message) {
    clear();
    _display->setCursor(0, 8);
    _display->print(title);
    _display->setCursor(0, 20);
    _display->print(message);
    display();
}

void DisplayManager::setCursor(int x, int y) {
    _display->setCursor(x, y);
}

void DisplayManager::print(const char* text) {
    _display->print(text);
}

void DisplayManager::printInverted(int x, int y, const char* text) {
    // Calculate text dimensions
    int16_t x1, y1;
    uint16_t w, h;
    _display->getTextBounds(text, x, y, &x1, &y1, &w, &h);
    
    // Fill background with white
    _display->fillRect(x, y, w, h + 2, WHITE);
    
    // Set text color to black and print
    _display->setTextColor(BLACK);
    _display->setCursor(x, y);
    _display->print(text);
    
    // Reset text color to white
    _display->setTextColor(WHITE);
}

void DisplayManager::fillInvertedRect(int x, int y, int w, int h) {
    _display->fillRect(x, y, w, h, WHITE);
}