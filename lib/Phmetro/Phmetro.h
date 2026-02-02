#pragma once

#include <Arduino.h>
#include <vector>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_ADS1X15.h>
#include <queue>

class Phmetro {
public:
    // slope e offset: pH = slope * tensao_media + offset (calibre com tampões pH 7 e pH 4)
    Phmetro(uint8_t adcAddress, float slope, float offset, int numReadings);
    void collectReading();
    float calculateAveragePh();
    float getAverageVoltage();  // tensão média em V (para calibração)

    bool isConnected() const { return adsFound; }

private:
    Adafruit_ADS1115 ads;
    std::queue<float> phValues;
    uint8_t adcAddress;
    float slope;
    float offset;
    int numReadings;
    int count = 0;
    bool adsFound = false;
};