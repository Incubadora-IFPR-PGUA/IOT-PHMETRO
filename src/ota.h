#pragma once

#include <WiFi.h>
#include <WiFiManager.h>
#include <ArduinoOTA.h>
#include <Update.h>  

void setupOTA() {
  WiFiManager wifiManager;
  wifiManager.autoConnect("ESP32-OTA"); 
  ArduinoOTA.setPassword("esp32senha"); 
  ArduinoOTA.setHostname("esp32-ota");

  ArduinoOTA.onStart([]() {
    Update.begin(UPDATE_SIZE_UNKNOWN);  // ← força tamanho dinâmico
    Serial.println("Iniciando OTA...");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA concluído!");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progresso: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Erro OTA [%u]\n", error);
  });

  ArduinoOTA.begin();
  Serial.println("OTA pronto!");
}