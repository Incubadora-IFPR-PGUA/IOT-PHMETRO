#include "Phmetro.h"

Phmetro::Phmetro(uint8_t adcAddress, float calibrationValue, int numReadings)
    : adcAddress(adcAddress), calibrationValue(calibrationValue), numReadings(numReadings), ads() {
    ads.setGain(GAIN_TWOTHIRDS);
    ads.begin(adcAddress);  // Configura o endereço I2C aqui, usando o método begin
}

void Phmetro::collectReading() {
    if (count < numReadings) {
        float voltage = (ads.readADC_SingleEnded(0) * 5.0) / 65536.0;
        phValues.push(voltage);
        if (phValues.size() > numReadings) phValues.pop();
        count++;
    }
}

float Phmetro::calculateAveragePh() {
    float sum = 0.0;
    std::queue<float> temp = phValues;
    while (!temp.empty()) {
        sum += temp.front();
        temp.pop();
    }
    return (-9.27272 * (sum / phValues.size())) + calibrationValue;
}