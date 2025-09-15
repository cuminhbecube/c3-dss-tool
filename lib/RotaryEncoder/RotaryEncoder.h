#ifndef ROTARYENCODER_H
#define ROTARYENCODER_H

#include <Arduino.h>

class RotaryEncoder {
private:
    int swPin;
    int dtPin;
    int clkPin;
    
    volatile bool lastClkState;
    volatile bool lastDtState;
    volatile long encoderPosition;
    volatile bool buttonPressed;
    volatile unsigned long lastButtonTime;
    volatile unsigned long lastEncoderTime;
    
    static const unsigned long DEBOUNCE_DELAY = 30; // ms - balanced for stability and speed
    static const unsigned long ENCODER_DELAY = 2;   // ms - slightly increased for stability
    
    // Static instance pointer for interrupt handling
    static RotaryEncoder* instance;
    
public:
    RotaryEncoder(int swPin, int dtPin, int clkPin);
    
    bool begin();
    void update();
    
    // Encoder functions
    long getPosition() const;
    void setPosition(long position);
    long getPositionChange();
    
    // Button functions
    bool isButtonPressed();
    bool wasButtonPressed(); // One-shot button press
    
    // Interrupt handlers
    static void IRAM_ATTR handleEncoder();
    static void IRAM_ATTR handleButton();
    
    void reset();
};

#endif