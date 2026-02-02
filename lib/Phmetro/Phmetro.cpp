#include "Phmetro.h"

Phmetro::Phmetro(uint8_t adcAddress, float calibrationValue, int numReadings)
    : adcAddress(adcAddress), calibrationValue(calibrationValue), numReadings(numReadings), ads() {
    
    adsFound = ads.begin(adcAddress);
    if (!adsFound) {
        Serial.println("ERRO: ADS1115 nao encontrado no barramento I2C");
        return;
    }
    Serial.println("ADS1115 conectado com sucesso");
    ads.setGain(GAIN_TWOTHIRDS);
}


void Phmetro::collectReading() {
    if (!adsFound) return;

    int16_t raw = ads.readADC_SingleEnded(0);
    float voltage = raw * 0.1875 / 1000.0; // conversão correta

    phValues.push(voltage);
    if (phValues.size() > numReadings) phValues.pop();
}


float Phmetro::calculateAveragePh() {
    if (phValues.empty()) return 0.0f;

    float sum = 0.0;
    std::queue<float> temp = phValues;
    while (!temp.empty()) {
        sum += temp.front();
        temp.pop();
    }
    return (-9.27272 * (sum / phValues.size())) + calibrationValue;
}