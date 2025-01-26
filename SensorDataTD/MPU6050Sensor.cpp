#include "MPU6050Sensor.h"
#include <Arduino.h>

Adafruit_MPU6050 MPU6050Sensor::_mpu;
SensorData MPU6050Sensor::_offset = {0};
bool MPU6050Sensor::_calibrated = false;

bool MPU6050Sensor::begin(int sda_pin, int scl_pin) {
    Wire.begin(sda_pin, scl_pin);
    if (!_mpu.begin()) return false;
    
    _mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
    _mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    return true;
}

void MPU6050Sensor::calibrate(unsigned long duration) {
    SensorData sum = {0};
    unsigned long start = millis();
    unsigned samples = 0;
    
    while(millis() - start < duration) {
        sensors_event_t a, g, temp;
        _mpu.getEvent(&a, &g, &temp);
        sum.x += a.acceleration.x;
        sum.y += a.acceleration.y;
        sum.z += a.acceleration.z;
        samples++;
        delay(10);
    }
    
    _offset.x = sum.x / samples;
    _offset.y = sum.y / samples;
    _offset.z = (sum.z / samples) - 9.81; // Subtract gravity
    _calibrated = true;
}

SensorData MPU6050Sensor::readData() {
    sensors_event_t a, g, temp;
    _mpu.getEvent(&a, &g, &temp);
    
    // Apply calibration
    SensorData raw = {
        a.acceleration.x - _offset.x,
        a.acceleration.y - _offset.y,
        a.acceleration.z - _offset.z
    };
    
    // Low-pass filter (adjust alpha for smoothing)
    static SensorData filtered = {0};
    const float alpha = 0.2; // 0 < alpha < 1 (lower = more smoothing)
    
    filtered.x = alpha * raw.x + (1 - alpha) * filtered.x;
    filtered.y = alpha * raw.y + (1 - alpha) * filtered.y;
    filtered.z = alpha * raw.z + (1 - alpha) * filtered.z;
    
    return filtered;
}