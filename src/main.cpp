#include <Arduino.h>
#include <Update.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <time.h>
#include <UrlEncode.h>
#include <Wire.h>
#include <SPI.h>
#include "../lib/Phmetro/Phmetro.h"
#include "Utils/WifiManager.h"
#include "../lib/WhatsApp/WhatsApp.h"
#include "Services/phApiSender.h"

WifiManager wifiConnect;
WhatsApp whatsapp;
phApiSender phSender;
Phmetro* phMetro = nullptr;

#define READY_PIN 4  // Defina o pino correto
volatile int contagem = 0; // Defina a variável contagem

// Calibração pH: use tampão pH 7 e pH 4, anote as tensões no Serial e calcule:
// slope = (7 - 4) / (V_pH7 - V_pH4)   e   offset = 7 - slope * V_pH7
#define CALIB_SLOPE   -9.27272f
#define CALIB_OFFSET  20.41272f

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 3600000; // 36000 1 hora

// Declaração da função onDataReady para interrupção
void IRAM_ATTR onDataReady() {
    contagem++;
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    wifiConnect.connect();

    Wire.begin(21, 22); // SDA, SCL
    Wire.setClock(100000);  
    Serial.println("I2C iniciado");

    pinMode(READY_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(READY_PIN), onDataReady, FALLING);
    
    phMetro = new Phmetro(0x48, CALIB_SLOPE, CALIB_OFFSET, 10);
    Serial.println("phMetro instanciado");
}


void loop() {
    phMetro->collectReading();

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi desconectado, tentando reconectar...");
        wifiConnect.connect();
        delay(5000);
    } else {
        if (millis() - lastSendTime >= sendInterval) {
            whatsapp.sendWhatsAppMessage("teste");
            lastSendTime = millis();
        }
        
        if (phMetro->isConnected()) {
            float phValue = phMetro->calculateAveragePh();
            float volts = phMetro->getAverageVoltage();
            Serial.printf("pH=%.2f  V=%.4f\n", phValue, volts);  // para calibração: anote V em tampão 7 e 4
            phSender.sendPhToApi(phValue);
        }
    }

    delay(10000);
}