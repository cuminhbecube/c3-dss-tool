#include <Arduino.h>
#include <Wire.h>
#include "SHTSensor.h"
#include "DisplayManager.h"
#include "BuzzerManager.h"
#include "FuelSensor.h"
#include "RotaryEncoder.h"

// Function declarations
void handleEncoderMenu(unsigned long currentTime);
void handleShortPress(unsigned long currentTime);
void handleSingleClick();
void handleDoubleClick();
void handleLongPress();
float calculateFuelPercentage(int currentLevel);

// Pin definitions for ESP32-C3
#define SDA_PIN 6
#define SCL_PIN 5
#define LED1_PIN 20    // LED trạng thái 1
#define LED2_PIN 21    // LED trạng thái 2
#define BUZZER_PIN 7   // Còi báo
#define FUEL_TX_PIN 1  // UART1 TX cho fuel sensor
#define FUEL_RX_PIN 2  // UART1 RX cho fuel sensor
#define ROTARY_SW_PIN 8   // Rotary encoder switch
#define ROTARY_DT_PIN 9   // Rotary encoder DT
#define ROTARY_CLK_PIN 10 // Rotary encoder CLK

// Create sensor, display and buzzer objects
SHTSensor sht1(0x44);  // SHT sensor at address 0x44
SHTSensor sht2(0x45);  // SHT sensor at address 0x45
DisplayManager display(128, 32, 0x3C);  // OLED 0.91" usually 128x32
BuzzerManager buzzer(BUZZER_PIN, 0);    // Buzzer on pin 7, PWM channel 0
FuelSensor fuelSensor(0x01);            // Fuel sensor at address 0x01
RotaryEncoder encoder(ROTARY_SW_PIN, ROTARY_DT_PIN, ROTARY_CLK_PIN); // Rotary encoder

// Timing variables
unsigned long lastReadTime = 0;
const unsigned long READ_INTERVAL = 2000; // Read every 2 seconds

// Sensor status
bool sht_sensor_available = false;
uint8_t sht_sensor_address = 0x00;  // Detected SHT address
bool fuel_sensor_available = false;

// Menu system
enum MenuState {
  MENU_STARTUP = 0,     // Checking sensors, show "DSS TOOL" if none
  MENU_MAIN,            // Main menu: Fuel + SHT info with highlighting
  MENU_FUEL_DETAIL,     // Detailed fuel information
  MENU_SHT_DETAIL,      // Large temperature/humidity display
  MENU_SETTING          // Setting menu: Set Full/Empty
};

enum MenuHighlight {
  HIGHLIGHT_FUEL = 0,
  HIGHLIGHT_SHT,
  HIGHLIGHT_COUNT
};

enum SettingOption {
  SETTING_SET_FULL = 0,
  SETTING_SET_EMPTY,
  SETTING_COUNT
};

MenuState currentMenuState = MENU_STARTUP;
MenuHighlight currentHighlight = HIGHLIGHT_FUEL;
SettingOption currentSetting = SETTING_SET_FULL;
unsigned long lastMenuActivity = 0;
unsigned long buttonPressStart = 0;
bool buttonPressed = false;
int clickCount = 0;
unsigned long lastClickTime = 0;
const unsigned long DOUBLE_CLICK_TIME = 300;  // 300ms for faster double click detection
const unsigned long LONG_PRESS_TIME = 3000;   // 3 seconds for long press
const unsigned long MENU_TIMEOUT_SECONDS = 30; // 30 seconds timeout
int menuTimeoutCounter = MENU_TIMEOUT_SECONDS;

// Display update flags for responsive UI
bool forceDisplayUpdate = false;
unsigned long lastDisplayUpdate = 0;
const unsigned long MIN_DISPLAY_INTERVAL = 50; // Minimum 50ms between display updates

// Hotswap detection variables
bool prev_sht_sensor_available = false;
bool prev_fuel_sensor_available = false;
unsigned long lastSensorCheck = 0;
const unsigned long SENSOR_CHECK_INTERVAL = 5000; // Check every 5 seconds

// Function declarations for hotswap detection
void checkSensorHotswap();
void onSensorConnected(const char* sensorName);
void onSensorDisconnected(const char* sensorName);

// LED control
void setLED1(bool state) {
  digitalWrite(LED1_PIN, state);
}

void setLED2(bool state) {
  digitalWrite(LED2_PIN, state);
}

// Handle rotary encoder for menu navigation
void handleEncoderMenu(unsigned long currentTime) {
  // Check for encoder rotation
  int positionChange = encoder.getPositionChange();
  if (positionChange != 0) {
    lastMenuActivity = currentTime;
    menuTimeoutCounter = MENU_TIMEOUT_SECONDS;
    forceDisplayUpdate = true; // Force immediate display update
    
    switch (currentMenuState) {
      case MENU_MAIN:
        // Navigate between Fuel and Setting highlight
        currentHighlight = (MenuHighlight)((currentHighlight + positionChange + HIGHLIGHT_COUNT) % HIGHLIGHT_COUNT);
        Serial.printf("Main menu highlight: %d\n", currentHighlight);
        break;
        
      case MENU_SETTING:
        // Navigate between Set Full and Set Empty
        currentSetting = (SettingOption)((currentSetting + positionChange + SETTING_COUNT) % SETTING_COUNT);
        Serial.printf("Setting option: %d\n", currentSetting);
        break;
        
      default:
        break;
    }
  }
  
  // Handle button press detection
  bool currentButtonState = encoder.wasButtonPressed();
  if (currentButtonState && !buttonPressed) {
    // Button just pressed
    buttonPressed = true;
    buttonPressStart = currentTime;
    lastMenuActivity = currentTime;
    menuTimeoutCounter = MENU_TIMEOUT_SECONDS;
  } else if (!currentButtonState && buttonPressed) {
    // Button just released
    buttonPressed = false;
    unsigned long pressDuration = currentTime - buttonPressStart;
    
    if (pressDuration >= LONG_PRESS_TIME) {
      // Long press (3+ seconds) - confirm setting
      handleLongPress();
    } else {
      // Short press - handle click counting for double click
      handleShortPress(currentTime);
    }
  }
  
  // Handle double click timeout
  if (clickCount > 0 && currentTime - lastClickTime > DOUBLE_CLICK_TIME) {
    if (clickCount == 1) {
      // Single click action
      handleSingleClick();
    }
    clickCount = 0;
  }
  
  // Handle menu timeout (return to main after inactivity)
  if (currentMenuState != MENU_MAIN && currentTime - lastMenuActivity >= 1000) {
    lastMenuActivity = currentTime;
    if (menuTimeoutCounter > 0) {
      menuTimeoutCounter--;
      if (menuTimeoutCounter == 0) {
        currentMenuState = MENU_MAIN;
        Serial.println("Menu timeout - returning to main menu");
      }
    }
  }
}

void handleShortPress(unsigned long currentTime) {
  clickCount++;
  lastClickTime = currentTime;
  
  if (clickCount == 2) {
    // Double click detected
    handleDoubleClick();
    clickCount = 0;
  }
}

void handleSingleClick() {
  buzzer.playSuccess();
  Serial.println("Single click detected");
  
  switch (currentMenuState) {
    case MENU_MAIN:
      if (currentHighlight == HIGHLIGHT_FUEL) {
        // Enter fuel detail mode
        currentMenuState = MENU_FUEL_DETAIL;
        Serial.println("Entering fuel detail mode");
      } else if (currentHighlight == HIGHLIGHT_SHT) {
        // Enter SHT large display mode
        currentMenuState = MENU_SHT_DETAIL;
        Serial.println("Entering SHT detail mode");
      }
      break;
      
    case MENU_FUEL_DETAIL:
      // Return to main menu
      currentMenuState = MENU_MAIN;
      Serial.println("Returning to main menu");
      break;
      
    case MENU_SHT_DETAIL:
      // Return to main menu
      currentMenuState = MENU_MAIN;
      Serial.println("Returning to main menu from SHT detail");
      break;
      
    case MENU_SETTING:
      // Return to main menu
      currentMenuState = MENU_MAIN;
      Serial.println("Returning to main menu from settings");
      break;
      
    default:
      break;
  }
}

void handleDoubleClick() {
  buzzer.playSuccess();
  buzzer.playSuccess(); // Double beep for double click
  Serial.println("Double click detected");
  
  switch (currentMenuState) {
    case MENU_MAIN:
      // Enter setting mode
      currentMenuState = MENU_SETTING;
      currentSetting = SETTING_SET_FULL; // Reset to first option
      Serial.println("Entering setting mode");
      break;
      
    default:
      break;
  }
}

void handleLongPress() {
  buzzer.playTemperatureAlert(); // Long beep for confirmation
  Serial.println("Long press detected");
  
  switch (currentMenuState) {
    case MENU_SETTING:
      // Send command to fuel sensor
      if (currentSetting == SETTING_SET_FULL) {
        Serial.println("Sending SET FULL command to fuel sensor");
        if (fuel_sensor_available && fuelSensor.setFullLevel()) {
          display.showError("SET FULL - OK");
        } else {
          display.showError("SET FULL - FAIL");
        }
      } else if (currentSetting == SETTING_SET_EMPTY) {
        Serial.println("Sending SET EMPTY command to fuel sensor");
        if (fuel_sensor_available && fuelSensor.setEmptyLevel()) {
          display.showError("SET EMPTY - OK");
        } else {
          display.showError("SET EMPTY - FAIL");
        }
      }
      
      // Show confirmation for 2 seconds
      delay(2000);
      
      // Return to main menu after command
      currentMenuState = MENU_MAIN;
      break;
      
    default:
      break;
  }
}

// Calculate fuel percentage based on min/max levels
float calculateFuelPercentage(int currentLevel) {
  if (!fuel_sensor_available || !fuelSensor.areLimitsValid() || currentLevel < 0) {
    return -1.0; // Invalid
  }
  
  float minLevel = fuelSensor.getLevelMinLiters();
  float maxLevel = fuelSensor.getLevelMaxLiters();
  
  if (maxLevel <= minLevel) {
    return -1.0; // Invalid range
  }
  
  float percentage = ((float)currentLevel - minLevel) / (maxLevel - minLevel) * 100.0;
  if (percentage < 0) percentage = 0;
  if (percentage > 100) percentage = 100;
  
  return percentage;
}

// Hotswap detection functions
void checkSensorHotswap() {
  unsigned long currentTime = millis();
  if (currentTime - lastSensorCheck < SENSOR_CHECK_INTERVAL) {
    return; // Not time to check yet
  }
  
  lastSensorCheck = currentTime;
  
  // Check SHT sensor hotswap
  bool current_sht_available = false;
  
  // Try to detect SHT sensor (quick check without full begin)
  Wire.beginTransmission(0x44);
  if (Wire.endTransmission() == 0) {
    current_sht_available = true;
    if (!sht_sensor_available && sht_sensor_address != 0x44) {
      // New SHT sensor detected at 0x44
      if (sht1.begin(SDA_PIN, SCL_PIN)) {
        sht_sensor_available = true;
        sht_sensor_address = 0x44;
        onSensorConnected("SHT at 0x44");
      }
    }
  } else {
    Wire.beginTransmission(0x45);
    if (Wire.endTransmission() == 0) {
      current_sht_available = true;
      if (!sht_sensor_available && sht_sensor_address != 0x45) {
        // New SHT sensor detected at 0x45
        if (sht2.begin(SDA_PIN, SCL_PIN)) {
          sht_sensor_available = true;
          sht_sensor_address = 0x45;
          onSensorConnected("SHT at 0x45");
        }
      }
    }
  }
  
  // Check if SHT sensor was disconnected
  if (prev_sht_sensor_available && !current_sht_available && sht_sensor_available) {
    sht_sensor_available = false;
    sht_sensor_address = 0;
    onSensorDisconnected("SHT");
  }
  
  // Check Fuel sensor hotswap (try basic communication)
  bool current_fuel_available = false;
  if (fuelSensor.isConnected()) {
    current_fuel_available = true;
    if (!fuel_sensor_available) {
      // Fuel sensor reconnected
      fuel_sensor_available = true;
      onSensorConnected("Fuel");
    }
  } else {
    if (fuel_sensor_available) {
      // Fuel sensor disconnected
      fuel_sensor_available = false;
      onSensorDisconnected("Fuel");
    }
  }
  
  // Update previous states
  prev_sht_sensor_available = current_sht_available;
  prev_fuel_sensor_available = current_fuel_available;
}

void onSensorConnected(const char* sensorName) {
  Serial.printf("Sensor connected: %s\n", sensorName);
  buzzer.playSensorFound(1);
  
  // Brief notification on display
  display.showNotification("SENSOR CONNECTED:", sensorName);
  delay(1500);
}

void onSensorDisconnected(const char* sensorName) {
  Serial.printf("Sensor disconnected: %s\n", sensorName);
  buzzer.playWarning();
  
  // Brief notification on display
  display.showNotification("SENSOR LOST:", sensorName);
  delay(1500);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Tool Fuel C3 - SHT Sensor Monitor");
  Serial.println("Initializing...");
  
  // Initialize LED pins
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  
  // Turn off LEDs initially
  setLED1(false);
  setLED2(false);
  
  // Initialize buzzer
  if (!buzzer.begin()) {
    Serial.println("Buzzer initialization failed!");
  }
  
  // Initialize rotary encoder
  if (!encoder.begin()) {
    Serial.println("Rotary encoder initialization failed!");
  }
  
  // Play startup sequence (LED + buzzer)
  for (int i = 0; i < 3; i++) {
    setLED1(true);
    delay(200);
    setLED1(false);
    setLED2(true);
    delay(200);
    setLED2(false);
  }
  buzzer.playStartupSequence();
  
  // Initialize display
  if (!display.begin(SDA_PIN, SCL_PIN)) {
    Serial.println("OLED display initialization failed!");
    // Error indication: fast blinking LED1
    for (int i = 0; i < 10; i++) {
      setLED1(true);
      delay(100);
      setLED1(false);
      delay(100);
    }
    while (1) delay(1000);
  }
  
  setLED1(true); // LED1 ON = Display OK
  
  display.showStartupMessage();
  delay(2000);
  
  // Initialize I2C for sensors (already done by display.begin)
  display.showConnecting();
  
  // Auto-detect SHT sensor (try 0x44 first, then 0x45)
  Serial.println("Auto-detecting SHT sensor...");
  if (sht1.begin(SDA_PIN, SCL_PIN)) {
    sht_sensor_available = true;
    sht_sensor_address = 0x44;
    Serial.println("SHT sensor found at 0x44");
    buzzer.playSensorFound(1);
  } else if (sht2.begin(SDA_PIN, SCL_PIN)) {
    sht_sensor_available = true; 
    sht_sensor_address = 0x45;
    Serial.println("SHT sensor found at 0x45");
    buzzer.playSensorFound(1);
  } else {
    Serial.println("No SHT sensor found at 0x44 or 0x45");
  }
  
  // Initialize Fuel Sensor (RS232)
  Serial.println("Initializing Fuel Sensor (RS232)...");
  fuel_sensor_available = fuelSensor.begin(FUEL_TX_PIN, FUEL_RX_PIN, 9600);
  if (fuel_sensor_available) {
    Serial.println("Fuel sensor (RS232) connected successfully!");
    buzzer.playSuccess(); // Success buzzer for fuel sensor connection
    delay(200);
    buzzer.playSuccess(); // Double beep for fuel sensor
    
    // Đọc limits từ fuel sensor
    Serial.println("Reading fuel sensor limits...");
    delay(500);
    if (fuelSensor.readLimits()) {
      Serial.printf("Fuel Limits - Max: %.1fL, Min: %.1fL\n", 
                    fuelSensor.getLevelMaxLiters(), fuelSensor.getLevelMinLiters());
    } else {
      Serial.println("Failed to read fuel sensor limits");
    }
  } else {
    Serial.println("Failed to connect fuel sensor (RS232)");
  }
  
  // Set LED2 status based on sensors
  setLED2(sht_sensor_available || fuel_sensor_available);
  
  if (!sht_sensor_available && !fuel_sensor_available) {
    display.showError("No sensors found!");
    Serial.println("ERROR: No sensors detected!");
    buzzer.playError(); // Error buzzer sequence
    currentMenuState = MENU_STARTUP; // Stay in startup to show "DSS TOOL"
  } else {
    int sensorCount = (sht_sensor_available ? 1 : 0) + (fuel_sensor_available ? 1 : 0);
    Serial.printf("Found %d sensor(s)\n", sensorCount);
    if (sht_sensor_address > 0) {
      Serial.printf("SHT sensor address: 0x%02X\n", sht_sensor_address);
    }
    currentMenuState = MENU_MAIN; // Go to main menu if sensors found
  }
  
  // Initialize previous sensor states for hotswap detection
  prev_sht_sensor_available = sht_sensor_available;
  prev_fuel_sensor_available = fuel_sensor_available;
  lastSensorCheck = millis();
  
  delay(1000);
}

void loop() {
  unsigned long currentTime = millis();
  
  // Check for sensor hotswap
  checkSensorHotswap();
  
  // Handle rotary encoder for menu navigation
  handleEncoderMenu(currentTime);
  
  // Blink LED1 to show system is running
  static unsigned long ledBlinkTime = 0;
  if (currentTime - ledBlinkTime >= 1000) { // Blink every 1 second
    ledBlinkTime = currentTime;
    static bool led1State = false;
    led1State = !led1State;
    setLED1(led1State);
  }
  
  // Variables for current sensor readings
  static float shtTemp = NAN, shtHum = NAN;
  static float fuelTemp = NAN;
  static int fuelLevel = -1;
  static bool readSuccess = false;
  
  if (currentTime - lastReadTime >= READ_INTERVAL) {
    lastReadTime = currentTime;
    
    // Reset values
    shtTemp = NAN; shtHum = NAN;
    fuelTemp = NAN;
    fuelLevel = -1;
    readSuccess = false;
    
    // Read SHT sensor if available
    if (sht_sensor_available) {
      bool shtReadSuccess = false;
      if (sht_sensor_address == 0x44) {
        shtReadSuccess = sht1.readData();
        if (shtReadSuccess) {
          shtTemp = sht1.getTemperature();
          shtHum = sht1.getHumidity();
        }
      } else if (sht_sensor_address == 0x45) {
        shtReadSuccess = sht2.readData();
        if (shtReadSuccess) {
          shtTemp = sht2.getTemperature();
          shtHum = sht2.getHumidity();
        }
      }
      
      if (shtReadSuccess) {
        Serial.printf("SHT (0x%02X): %.1f°C, %.0f%%\n", sht_sensor_address, shtTemp, shtHum);
        readSuccess = true;
      } else {
        Serial.printf("Failed to read SHT sensor at 0x%02X\n", sht_sensor_address);
      }
    }
    
    // Read fuel sensor (RS232) if available
    if (fuel_sensor_available) {
      if (fuelSensor.readSensorData()) {
        fuelTemp = fuelSensor.getTemperature();
        fuelLevel = fuelSensor.getFuelLiters();
        Serial.printf("Fuel Sensor (RS232): %.1f°C, %dL\n", fuelTemp, fuelLevel);
        readSuccess = true;
      } else {
        Serial.println("Failed to read fuel sensor (RS232)");
      }
    }
    
    // Update LED2 based on read success
    setLED2(readSuccess);
    
    Serial.println("---");
  }
  
  // Update display immediately when encoder changes or periodically
  bool shouldUpdateDisplay = forceDisplayUpdate || 
                           (currentTime - lastDisplayUpdate >= MIN_DISPLAY_INTERVAL);
  
  if (shouldUpdateDisplay) {
    lastDisplayUpdate = currentTime;
    forceDisplayUpdate = false;
    
    // Update display based on current menu state
    switch (currentMenuState) {
      case MENU_STARTUP:
        // Show "DSS TOOL" when no sensors available
        display.showDSSTool();
        break;
        
      case MENU_MAIN:
        // Main menu with highlighting
        display.showMainMenu(shtTemp, shtHum, fuelTemp, fuelLevel, 
                           (int)currentHighlight, sht_sensor_available, fuel_sensor_available);
        break;
        
      case MENU_FUEL_DETAIL:
        // Detailed fuel information
        if (fuel_sensor_available) {
          display.showFuelDetails(fuelTemp, fuelLevel, 
                                fuelSensor.areLimitsValid() ? fuelSensor.getLevelMax() : -1,
                                fuelSensor.areLimitsValid() ? fuelSensor.getLevelMin() : -1);
        } else {
          display.showError("No fuel sensor");
        }
        break;
        
      case MENU_SHT_DETAIL:
        // Large SHT temperature and humidity display
        if (sht_sensor_available) {
          display.showSHTLargeDisplay(shtTemp, shtHum);
        } else {
          display.showError("No SHT sensor");
        }
        break;
        
      case MENU_SETTING:
        // Setting menu
        display.showSettingMenu((int)currentSetting);
        break;
    }
  }
  
  // Temperature and fuel level alerts (only when we have fresh sensor data)
  if (currentTime - lastReadTime < READ_INTERVAL + 100) {
    // Beep if temperature is too high (SHT: 35°C, Fuel: 80°C)
    if ((shtTemp > 35.0 && !isnan(shtTemp)) || (fuelTemp > 80.0 && !isnan(fuelTemp))) {
      buzzer.playTemperatureAlert(); // Temperature warning buzzer
    }
    
    // Check fuel level limits and alert
    if (fuel_sensor_available && fuelSensor.areLimitsValid() && fuelLevel >= 0) {
      float levelMaxLiters = fuelSensor.getLevelMaxLiters();
      float levelMinLiters = fuelSensor.getLevelMinLiters();
      
      if (fuelLevel <= levelMinLiters + 5.0) { // Alert when within 5L of minimum
        Serial.println("WARNING: Fuel level near minimum!");
        buzzer.playTemperatureAlert(); // Use temperature alert sound for fuel warning
      } else if (fuelLevel >= levelMaxLiters - 5.0) { // Alert when within 5L of maximum
        Serial.println("WARNING: Fuel level near maximum!");
        buzzer.playTemperatureAlert();
      }
    }
  }
  
  delay(10); // Reduced delay for better encoder responsiveness
}