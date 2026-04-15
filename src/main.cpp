#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <soc/rtc_cntl_reg.h>
#include <Adafruit_ADS1X15.h>
#include "../lib/WhatsApp/WhatsApp.h"
#include "Services/phApiSender.h"

#define READY_PIN 4
#define LED_PIN 2

Adafruit_ADS1115 ads;
byte adsAddress;
WhatsApp whatsapp;
phApiSender phSender;

unsigned long lastSendTime = 0;
unsigned long lastPhRead = 0;
unsigned long lastBlink = 0;

const unsigned long sendInterval = 3600000;
const unsigned long phInterval = 10000;

bool ledState = false;
bool adsOk = false;

float readPH() {
    float sum = 0;
    int samples = 10;

    for (int i = 0; i < samples; i++) {
        int16_t raw = ads.readADC_SingleEnded(0);
        sum += ads.computeVolts(raw);
        delay(50);
    }

    float voltage = sum / samples;
    float ph = -5.8606 * voltage + 22.5536;
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
    delay(100);

    // WiFiManager wm;
    // wm.setConfigPortalTimeout(180); 
    // if (!wm.autoConnect("ESP32-PHmetro")) {
    //     Serial.println("Falha ao conectar no WiFi. Reiniciando...");
    //     ESP.restart();
    // }
    // Serial.println("WiFi conectado! IP: " + WiFi.localIP().toString());

    Wire.begin(21, 22);
    Wire.setClock(100000);
    Wire.setTimeOut(50);
    delay(100);

    ads.setGain(GAIN_TWOTHIRDS);

    adsOk = initADS();
    if (!adsOk) {
        Serial.println("ADS não encontrado no setup. Vai tentar novamente no loop...");
    }
}

void loop() {
    //led
    if (millis() - lastBlink >= 5000) {
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
        lastBlink = millis();
    }

    if (millis() - lastPhRead >= phInterval) {
        lastPhRead = millis();

        if (!adsOk) {
            Serial.println("ADS offline, tentando reiniciar...");
            adsOk = initADS();
        }

        if (adsOk) {
            float phValue = readPH();
            //phSender.sendPhToApi(phValue, 2);
            if (millis() - lastSendTime >= sendInterval) {
                //whatsapp.sendWhatsAppMessage("pH atual: " + String(phValue, 2));
                lastSendTime = millis();
            }
        }
    }
}