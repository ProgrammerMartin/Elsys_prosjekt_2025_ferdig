#line 1 "/Users/martinflataker/Desktop/Prog_Semester_4/Elsys_semester_4_samlet/27_april/Arduino/Prosjekt_27_april_kl_15/piezo.h"
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