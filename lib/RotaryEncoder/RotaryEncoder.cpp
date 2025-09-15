#include "RotaryEncoder.h"

// Static member initialization
RotaryEncoder* RotaryEncoder::instance = nullptr;

RotaryEncoder::RotaryEncoder(int swPin, int dtPin, int clkPin) 
    : swPin(swPin), dtPin(dtPin), clkPin(clkPin),
      lastClkState(false), lastDtState(false), encoderPosition(0),
      buttonPressed(false), lastButtonTime(0), lastEncoderTime(0) {
    instance = this;
}

bool RotaryEncoder::begin() {
    // Configure pins
    pinMode(swPin, INPUT_PULLUP);
    pinMode(dtPin, INPUT_PULLUP);
    pinMode(clkPin, INPUT_PULLUP);
    
    // Read initial states
    delay(10); // Small delay to stabilize
    lastClkState = digitalRead(clkPin);
    lastDtState = digitalRead(dtPin);
    
    // Attach interrupts - using CHANGE for both CLK and DT for better detection
    attachInterrupt(digitalPinToInterrupt(clkPin), handleEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(dtPin), handleEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(swPin), handleButton, FALLING);
    
    Serial.println("RotaryEncoder initialized with improved stability");
    Serial.printf("Pins - SW: %d, DT: %d, CLK: %d\n", swPin, dtPin, clkPin);
    Serial.printf("Initial states - CLK: %d, DT: %d\n", lastClkState, lastDtState);
    
    return true;
}

void RotaryEncoder::update() {
    // This method can be called in main loop for additional processing
    // Currently, most work is done in interrupts
}

long RotaryEncoder::getPosition() const {
    return encoderPosition;
}

void RotaryEncoder::setPosition(long position) {
    noInterrupts();
    encoderPosition = position;
    interrupts();
}

long RotaryEncoder::getPositionChange() {
    static long lastPosition = 0;
    static unsigned long lastChangeTime = 0;
    
    long currentPosition = encoderPosition;
    long change = currentPosition - lastPosition;
    
    // Filter out very rapid changes that might be noise
    unsigned long currentTime = millis();
    if (change != 0) {
        if (currentTime - lastChangeTime < 10) { // Minimum 10ms between changes
            return 0; // Ignore rapid changes
        }
        lastChangeTime = currentTime;
    }
    
    lastPosition = currentPosition;
    
    // Limit change to reasonable values (-2 to +2) to avoid jumps
    if (change > 2) change = 2;
    if (change < -2) change = -2;
    
    return change;
}

bool RotaryEncoder::isButtonPressed() {
    return !digitalRead(swPin); // Active low
}

bool RotaryEncoder::wasButtonPressed() {
    if (buttonPressed) {
        buttonPressed = false;
        return true;
    }
    return false;
}

void RotaryEncoder::reset() {
    noInterrupts();
    encoderPosition = 0;
    buttonPressed = false;
    interrupts();
}

// Static interrupt handlers
void IRAM_ATTR RotaryEncoder::handleEncoder() {
    if (instance == nullptr) return;
    
    unsigned long currentTime = millis();
    if (currentTime - instance->lastEncoderTime < ENCODER_DELAY) {
        return; // Debounce
    }
    
    bool clkState = digitalRead(instance->clkPin);
    bool dtState = digitalRead(instance->dtPin);
    
    // Only process on CLK falling edge for more reliable detection
    if (clkState != instance->lastClkState && clkState == LOW) {
        instance->lastEncoderTime = currentTime;
        
        // Determine direction based on DT state when CLK falls
        if (dtState == HIGH) {
            instance->encoderPosition++; // Clockwise
        } else {
            instance->encoderPosition--; // Counter-clockwise
        }
        
        // Update states
        instance->lastClkState = clkState;
        instance->lastDtState = dtState;
    } else {
        // Still update states for next comparison
        instance->lastClkState = clkState;
        instance->lastDtState = dtState;
    }
}

void IRAM_ATTR RotaryEncoder::handleButton() {
    if (instance == nullptr) return;
    
    unsigned long currentTime = millis();
    if (currentTime - instance->lastButtonTime > DEBOUNCE_DELAY) {
        instance->buttonPressed = true;
        instance->lastButtonTime = currentTime;
    }
}