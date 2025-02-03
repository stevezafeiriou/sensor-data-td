#ifndef MPU6050_SENSOR_H
#define MPU6050_SENSOR_H

#include <Adafruit_MPU6050.h>
#include <Wire.h>

// The SensorData struct now holds both acceleration and gyro values.
struct SensorData {
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
};

class MPU6050Sensor {
public:
    static bool begin(int sda_pin, int scl_pin);
    static SensorData readData();
    static void calibrate(unsigned long duration = 3000);
    
private:
    static Adafruit_MPU6050 _mpu;
    static SensorData _offset;  // Offsets for acceleration and gyro
    static bool _calibrated;
};

#endif
