#line 1 "/Users/martinflataker/Desktop/Prog_Semester_4/Elsys_semester_4_samlet/27_april/Arduino/Prosjekt_27_april_kl_15/gyAc.h"
#ifndef GYAC_H
#define GYAC_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Deklarerer nødvendige variabler og objekter
extern Adafruit_MPU6050 mpu;

// extern const int BUFFER_SIZE;  // Deklarer konstanten som ekstern

struct __attribute__((packed)) mpuDataType {
    float accelX;
    float accelY;
    float accelZ;
    float gyroX;
    float gyroY;
    float gyroZ;
    uint16_t timestamp;
};

mpuDataType GyAc();

#endif