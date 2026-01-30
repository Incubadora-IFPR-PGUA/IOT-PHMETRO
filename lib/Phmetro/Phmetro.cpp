#include "Phmetro.h"

Phmetro::Phmetro(uint8_t adcAddress, float calibrationValue, int numReadings)
    : adcAddress(adcAddress), calibrationValue(calibrationValue), numReadings(numReadings), ads() {
    
    if (!ads.begin(adcAddress)) {
        Serial.println("ERRO: ADS1115 nao encontrado no barramento I2C");
    } else {
        Serial.println("ADS1115 conectado com sucesso");
    }

    ads.setGain(GAIN_TWOTHIRDS);
}


void Phmetro::collectReading() {
    int16_t raw = ads.readADC_SingleEnded(0);
    float voltage = raw * 0.1875 / 1000.0; // conversão correta

    phValues.push(voltage);
    if (phValues.size() > numReadings) phValues.pop();
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