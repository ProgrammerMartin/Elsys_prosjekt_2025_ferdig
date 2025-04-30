#include "gyAc.h"

static float offax = 0.4, offay = -0.17, offaz = -1.02;
static float offgx = 0.01, offgy = -0.62, offgz = -0.19;

// const int BUFFER_SIZE = 100;  // Sett en passende størrelse

float ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0;

mpuDataType GyAc() {
    // static char buffer[BUFFER_SIZE];  // Statisk buffer
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    if (a.acceleration.x == 0 && a.acceleration.y == 0 && a.acceleration.z == 0){
        ESP.restart();
    }

    ax = a.acceleration.x - offax;
    ay = a.acceleration.y - offay;
    az = a.acceleration.z - offaz;
    gx = g.gyro.x - offgx;
    gy = g.gyro.y - offgy;
    gz = g.gyro.z - offgz;

    mpuDataType dataInStruct{ax, ay, az, gx, gy, gz};  // Legger ikke til timestampet her
  
    // Printer alle verdier til Serial Monitor
    /*
    Serial.print("Akselerasjon (fra Structen) justert med offset: X=");
    Serial.print(dataInStruct.accelX);
    Serial.print(", Y=");
    Serial.print(dataInStruct.accelY);
    Serial.print(", Z=");
    Serial.println(dataInStruct.accelZ);
    Serial.print("Timestamp: ");
    Serial.println(dataInStruct.timestamp);
    */

    /*
    snprintf(buffer, BUFFER_SIZE, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
            ax, ay, az, gx, gy, gz);
    
    return buffer;  // Retur av statisk buffer (unngår heap-problemer)
    */

    return dataInStruct;
}
