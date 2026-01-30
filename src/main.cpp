#include <Arduino.h>
#include <Update.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <time.h>
#include <UrlEncode.h>
#include <SPI.h>
#include "../lib/Phmetro/Phmetro.h"
#include "Utils/WifiManager.h"
#include "../lib/WhatsApp/WhatsApp.h"
#include "Services/phApiSender.h"

#define READY_PIN 4  // Defina o pino correto
volatile int contagem = 0; // Defina a variável contagem

WifiManager wifiConnect;
Phmetro phMetro(0x49, 20.41272, 10); // Instancie phMetro com os parâmetros corretos
WhatsApp whatsapp;
phApiSender phSender;

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

    pinMode(READY_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(READY_PIN), onDataReady, FALLING);
}

void loop() {
    phMetro.collectReading();

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi desconectado, tentando reconectar...");
        wifiConnect.connect();
        delay(5000);
    } else {
        if (millis() - lastSendTime >= sendInterval) {
            whatsapp.sendWhatsAppMessage("teste");
            lastSendTime = millis();
        }
        
        float phValue = phMetro.calculateAveragePh();
        phSender.sendPhToApi(phValue); // Ajuste o valor conforme necessário
    }

    // Serial.println(5);
    delay(10000);
}