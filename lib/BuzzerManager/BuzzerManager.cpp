#include "BuzzerManager.h"

BuzzerManager::BuzzerManager(uint8_t pin, uint8_t channel) {
    _pin = pin;
    _channel = channel;
    _defaultFreq = 2700;  // Standard frequency for 8530 buzzer (2.7kHz)
    _dutyCycle = 128;     // 50% duty cycle for optimal sound
    _resolution = 8;      // 8-bit resolution (0-255)
}

bool BuzzerManager::begin() {
    pinMode(_pin, OUTPUT);
    setupPWM();
    off(); // Start with buzzer off
    return true;
}

void BuzzerManager::setupPWM() {
    ledcSetup(_channel, _defaultFreq, _resolution);
    ledcAttachPin(_pin, _channel);
}

void BuzzerManager::on() {
    ledcWrite(_channel, _dutyCycle);
}

void BuzzerManager::off() {
    ledcWrite(_channel, 0);
}

void BuzzerManager::beep(int duration_ms) {
    on();
    delay(duration_ms);
    off();
}

void BuzzerManager::playTone(int frequency, int duration_ms) {
    ledcWriteTone(_channel, frequency);
    delay(duration_ms);
    ledcWrite(_channel, 0);
}

void BuzzerManager::beepTone(int frequency, int duration_ms) {
    playTone(frequency, duration_ms);
}

void BuzzerManager::playMelody(int frequencies[], int durations[], int length) {
    for (int i = 0; i < length; i++) {
        if (frequencies[i] > 0) {
            playTone(frequencies[i], durations[i]);
        } else {
            delay(durations[i]); // Rest note
        }
        delay(50); // Small gap between notes
    }
}

void BuzzerManager::playStartupSequence() {
    // Ascending melody for startup
    int frequencies[] = {1000, 1500, 2000};
    int durations[] = {150, 150, 200};
    playMelody(frequencies, durations, 3);
}

void BuzzerManager::playSensorFound(int sensorNumber) {
    // Different tones for different sensors
    if (sensorNumber == 1) {
        playTone(1800, 100); // Higher tone for sensor 1
    } else if (sensorNumber == 2) {
        playTone(2200, 100); // Even higher tone for sensor 2
    } else {
        playTone(2000, 100); // Default tone
    }
}

void BuzzerManager::playSuccess() {
    // High tone for success
    playTone(2500, 300);
}

void BuzzerManager::playError() {
    // Descending tones for error
    int frequencies[] = {800, 600, 400};
    int durations[] = {200, 200, 200};
    
    for (int i = 0; i < 3; i++) {
        playTone(frequencies[i], durations[i]);
        delay(200);
    }
}

void BuzzerManager::playWarning() {
    // Double beep for warning
    playTone(2500, 100);
    delay(100);
    playTone(2500, 100);
}

void BuzzerManager::playTemperatureAlert() {
    // High-pitched double beep for temperature alert
    playTone(3000, 100);
    delay(100);
    playTone(3000, 100);
}

void BuzzerManager::playReadError() {
    // Low tone for read error
    playTone(500, 200);
}

void BuzzerManager::setDefaultFrequency(int frequency) {
    _defaultFreq = frequency;
    setupPWM();
}

void BuzzerManager::setDutyCycle(int duty) {
    if (duty >= 0 && duty <= 255) {
        _dutyCycle = duty;
    }
}