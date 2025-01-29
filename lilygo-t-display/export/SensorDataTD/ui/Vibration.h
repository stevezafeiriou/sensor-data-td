#ifndef VIBRATION_H
#define VIBRATION_H

#include <Arduino.h>

class Vibration {
public:
    static void begin(uint8_t pin);
    static void update(bool active);
    
private:
    static uint8_t _pin;
    static enum State { OFF, ON, PAUSE } _state;
    static unsigned long _last_change;
};

#endif