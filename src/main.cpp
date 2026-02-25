/**
 * @file main.cpp
 * @author Josue Henrique (josue21henrique@gmail.com)
 * @brief Firmware para um monitor de pH com ESP32, módulo ADS1115
 *             e sensor pH-4502C.
 * @version 0.1
 * @date 2026-25-02
 *
 * @copyright Copyright (c) 2026
 *
 */

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <soc/rtc_cntl_reg.h>

#include <Adafruit_ADS1X15.h>
#include "Utils/WifiManager.h"
#include "../lib/WhatsApp/WhatsApp.h"
#include "Services/phApiSender.h"

#define READY_PIN 4

Adafruit_ADS1115 ads;

WifiManager wifiConnect;
WhatsApp whatsapp;
phApiSender phSender;

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 3600000; // 1 hora

float readPH() {
    int16_t raw = ads.readADC_SingleEnded(0);
    float voltage = ads.computeVolts(raw);

    // Equação calibrada com seus pontos reais
    float ph = -14.53 * voltage + 47.69;

    // Serial.print("Tensão: ");
    // Serial.print(voltage, 4);
    // Serial.print(" | pH: ");
    // Serial.println(ph, 2);
    return ph;
}

void setup() {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    Serial.begin(115200);
    delay(300);

    Wire.begin(21, 22);
    Wire.setClock(100000);
    delay(50);

    ads.setGain(GAIN_TWOTHIRDS); // ±6.144V

    if (!ads.begin(0x49)) {
        Serial.println("Falha ao iniciar o ADS.");
        while (1);
    }

    wifiConnect.connect();
    pinMode(READY_PIN, INPUT);
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi desconectado, reconectando...");
        wifiConnect.connect();
        delay(5000);
        return;
    }

    float phValue = readPH();
    phSender.sendPhToApi(phValue);

    if (millis() - lastSendTime >= sendInterval) {
        String msg = "pH atual: " + String(phValue, 2);
        whatsapp.sendWhatsAppMessage(msg);
        lastSendTime = millis();
    }
    delay(10000);
}