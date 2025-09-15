#ifndef BUZZERMANAGER_H
#define BUZZERMANAGER_H

#include <Arduino.h>

class BuzzerManager {
public:
    BuzzerManager(uint8_t pin = 7, uint8_t channel = 0);
    bool begin();
    
    // Basic control
    void on();
    void off();
    void beep(int duration_ms = 100);
    void beepTone(int frequency, int duration_ms);
    
    // Predefined sounds
    void playStartupSequence();
    void playSensorFound(int sensorNumber = 1);
    void playSuccess();
    void playError();
    void playWarning();
    void playTemperatureAlert();
    void playReadError();
    
    // Configuration
    void setDefaultFrequency(int frequency);
    void setDutyCycle(int duty);
    
private:
    uint8_t _pin;
    uint8_t _channel;
    int _defaultFreq;
    int _dutyCycle;
    int _resolution;
    
    void setupPWM();
    void playTone(int frequency, int duration_ms);
    void playMelody(int frequencies[], int durations[], int length);
};

#endif // BUZZERMANAGER_H