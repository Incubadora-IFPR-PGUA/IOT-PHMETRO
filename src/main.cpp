/**
 * @file main.cpp
 * @author Josue Henrique (josue21henrique@gmail.com)
 * @brief Firmware para um monitor de pH com ESP32, módulo ADS1115
 *             e sensor pH-4502C.
 * @version 0.2
 * @date 2026-25-02
 *
 * @copyright Copyright (c) 2026
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
#include "../lib/OtaManager/OtaManager.h"

#define READY_PIN 4
#define FIRMWARE_VERSION   "1.0.1"

#define LED_PIN 2 //remover depois

// ─── URLs do seu servidor ─────────────────────────────────────────────────────
#define OTA_VERSION_URL    "https://firmware.incubadoraifpr.com.br/version.txt"
#define OTA_FIRMWARE_URL   "https://firmware.incubadoraifpr.com.br/.pio/build/esp32dev/firmware.bin"

const unsigned long OTA_CHECK_INTERVAL = 3600000;
unsigned long lastOtaCheck = 0;

Adafruit_ADS1115 ads;
byte adsAddress;
WifiManager wifiConnect;
WhatsApp whatsapp;
phApiSender phSender;
OtaManager otaManager(OTA_VERSION_URL, OTA_FIRMWARE_URL, FIRMWARE_VERSION);

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 3600000; // 1 hora

unsigned long lastBlink = 0;
bool ledState = false;

float readPH() {
    float sum = 0;
    int samples = 10;

    for (int i = 0; i < samples; i++) {
        int16_t raw = ads.readADC_SingleEnded(0);
        sum += ads.computeVolts(raw);
        delay(50);
    }

    float voltage = sum / samples;
    float ph = -5.4545 * voltage + 21.29;
    ph = round(ph * 10.0) / 10.0;

    Serial.print("Tensão média: ");
    Serial.print(voltage, 4);
    Serial.print(" | pH: ");
    Serial.println(ph, 1);
    return ph;
}

bool initADS() {
    if (ads.begin(0x49)) {
        adsAddress = 0x49;
        Serial.println("ADS iniciado em 0x49");
        return true;
    }
    if (ads.begin(0x48)) {
        adsAddress = 0x48;
        Serial.println("ADS iniciado em 0x48");
        return true;
    }
    return false;
}

void setup() {
    pinMode(READY_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    Serial.begin(115200);
    delay(300);

    Serial.printf("\n[BOOT] Versão do firmware: %s\n", FIRMWARE_VERSION);

    Wire.begin(21, 22);
    Wire.setClock(100000);
    delay(50);

    ads.setGain(GAIN_TWOTHIRDS);

    if (!initADS()) {
        Serial.println("Falha ao iniciar o ADS.");
        //while (1);
    }

    wifiConnect.connect();
    pinMode(READY_PIN, INPUT);

    // Verifica OTA logo ao iniciar
    otaManager.checkAndUpdate();
    lastOtaCheck = millis();
}

void loop() {
    // Pisca LED sem travar o loop
    if (millis() - lastBlink >= 50) {
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
        lastBlink = millis();
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi desconectado, reconectando...");
        wifiConnect.connect();
        delay(5000);
        return;
    }

    if (millis() - lastOtaCheck >= OTA_CHECK_INTERVAL) {
        otaManager.checkAndUpdate();
        lastOtaCheck = millis();
    }

    // float phValue = readPH();
    // int idEsp32 = 2;

    // phSender.sendPhToApi(phValue, idEsp32);

    // if (millis() - lastSendTime >= sendInterval) {
    //     String msg = "pH atual: " + String(phValue, 2);
    //     whatsapp.sendWhatsAppMessage("teste");
    //     lastSendTime = millis();
    // }

    // delay(10000);
}