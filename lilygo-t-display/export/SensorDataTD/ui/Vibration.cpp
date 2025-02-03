#include "Vibration.h"

uint8_t Vibration::_pin = 10;
Vibration::State Vibration::_state = OFF;
unsigned long Vibration::_last_change = 0;

void Vibration::begin(uint8_t pin) {
    _pin = pin;
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
}

void Vibration::update(bool active) {
    if(!active) {
        if(_state != OFF) {
            digitalWrite(_pin, LOW);
            _state = OFF;
        }
        return;
    }

    unsigned long now = millis();
    
    switch(_state) {
        case OFF:
            digitalWrite(_pin, HIGH);
            _state = ON;
            _last_change = now;
            break;
            
        case ON:
            if(now - _last_change >= 250) {
                digitalWrite(_pin, LOW);
                _state = PAUSE;
                _last_change = now;
            }
            break;
            
        case PAUSE:
            if(now - _last_change >= 250) {
                digitalWrite(_pin, HIGH);
                _state = ON;
                _last_change = now;
            }
            break;
    }
}