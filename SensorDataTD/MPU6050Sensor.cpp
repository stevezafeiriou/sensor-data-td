#include "MPU6050Sensor.h"
#include <Arduino.h>

Adafruit_MPU6050 MPU6050Sensor::_mpu;
SensorData MPU6050Sensor::_offset = {0, 0, 0, 0, 0, 0};
bool MPU6050Sensor::_calibrated = false;

bool MPU6050Sensor::begin(int sda_pin, int scl_pin) {
    Wire.begin(sda_pin, scl_pin);
    if (!_mpu.begin()) return false;
    
    _mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
    _mpu.setGyroRange(MPU6050_RANGE_500_DEG); // Set an appropriate gyro range
    _mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    return true;
}

void MPU6050Sensor::calibrate(unsigned long duration) {
    SensorData sum = {0, 0, 0, 0, 0, 0};
    unsigned long start = millis();
    unsigned samples = 0;
    
    while (millis() - start < duration) {
        sensors_event_t a, g, temp;
        _mpu.getEvent(&a, &g, &temp);
        sum.ax += a.acceleration.x;
        sum.ay += a.acceleration.y;
        sum.az += a.acceleration.z;
        sum.gx += g.gyro.x;
        sum.gy += g.gyro.y;
        sum.gz += g.gyro.z;
        samples++;
        delay(10);
    }
    
    _offset.ax = sum.ax / samples;
    _offset.ay = sum.ay / samples;
    _offset.az = (sum.az / samples) - 9.81; // Subtract gravity
    _offset.gx = sum.gx / samples;
    _offset.gy = sum.gy / samples;
    _offset.gz = sum.gz / samples;
    
    _calibrated = true;
}

SensorData MPU6050Sensor::readData() {
    sensors_event_t a, g, temp;
    _mpu.getEvent(&a, &g, &temp);
    
    SensorData raw;
    raw.ax = a.acceleration.x - _offset.ax;
    raw.ay = a.acceleration.y - _offset.ay;
    raw.az = a.acceleration.z - _offset.az;
    raw.gx = g.gyro.x - _offset.gx;
    raw.gy = g.gyro.y - _offset.gy;
    raw.gz = g.gyro.z - _offset.gz;
    
    // Apply a simple low-pass filter to smooth the data.
    static SensorData filtered = {0, 0, 0, 0, 0, 0};
    const float alpha = 0.2; // Lower values give more smoothing.
    filtered.ax = alpha * raw.ax + (1 - alpha) * filtered.ax;
    filtered.ay = alpha * raw.ay + (1 - alpha) * filtered.ay;
    filtered.az = alpha * raw.az + (1 - alpha) * filtered.az;
    filtered.gx = alpha * raw.gx + (1 - alpha) * filtered.gx;
    filtered.gy = alpha * raw.gy + (1 - alpha) * filtered.gy;
    filtered.gz = alpha * raw.gz + (1 - alpha) * filtered.gz;
    
    return filtered;
}
