#ifndef MPU6050_SENSOR_H
#define MPU6050_SENSOR_H

#include <Adafruit_MPU6050.h>
#include <Wire.h>

struct SensorData {
    float x;
    float y;
    float z;
};

class MPU6050Sensor {
public:
    static bool begin(int sda_pin, int scl_pin);
    static SensorData readData();
    static void calibrate(unsigned long duration = 3000);
    
private:
    static Adafruit_MPU6050 _mpu;
    static SensorData _offset;
    static bool _calibrated;
};

#endif