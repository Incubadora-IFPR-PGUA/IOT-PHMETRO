#pragma once

#include <Arduino.h>
#include <vector>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_ADS1X15.h>
#include <queue>

class Phmetro {
public:
    Phmetro(uint8_t adcAddress, float calibrationValue, int numReadings);
    void collectReading();
    float calculateAveragePh();

private:
    Adafruit_ADS1115 ads;
    std::queue<float> phValues;
    uint8_t adcAddress;
    float calibrationValue;
    int numReadings;
    int count = 0;
};