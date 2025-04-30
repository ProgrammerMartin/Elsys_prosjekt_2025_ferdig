#include "piezo.h"

// Konstruktør
PiezoSensor::PiezoSensor(uint8_t pin) : piezoPin(pin) {}

// Medlemsfunksjon som leser piezo-verdi
piezoDataType PiezoSensor::readPiezo(){
    int value = analogRead(this->piezoPin);
    piezoDataType dataInStruct{value}; // Setter timestampen i piezo sampling tasken
    return dataInStruct;
}