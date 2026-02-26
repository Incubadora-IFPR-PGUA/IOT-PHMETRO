#include "phApiSender.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>

void phApiSender::sendPhToApi(float phValue, int id) {
    String URL = "https://apivitrineprototipos.incubadoraifpr.com.br/inserirPh";
    HTTPClient http;

    http.begin(URL);
    http.addHeader("Content-Type", "application/json");

    DynamicJsonDocument jsonDoc(200);
    jsonDoc["ph"] = phValue;
    jsonDoc["id_esp_macAdress"] = id;
    String jsonPayload;
    serializeJson(jsonDoc, jsonPayload);

    int httpResponseCode = http.POST(jsonPayload);    

    if (httpResponseCode == 201 || httpResponseCode == 200) {
        String payload = http.getString();
        Serial.println("Resposta da API:");
        Serial.println(payload);
    } else {
        Serial.print("Erro ao tentar enviar os dados! Código: ");
        Serial.println(httpResponseCode);
    }

    http.end();
}
