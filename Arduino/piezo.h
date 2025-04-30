#ifndef PIEZO_H
#define PIEZO_H

#include <Arduino.h>

struct piezoDataType {
    uint16_t value;
    uint16_t timestamp;
};

class PiezoSensor {
    private:
        const uint8_t piezoPin;
    public:
        explicit PiezoSensor(uint8_t pin);
        piezoDataType readPiezo();
};

#endif