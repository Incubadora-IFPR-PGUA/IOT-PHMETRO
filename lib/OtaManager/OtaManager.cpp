#include "OtaManager.h"
#include <WiFiClientSecure.h>

OtaManager::OtaManager(const char* versionUrl, const char* firmwareUrl, const char* currentVersion)
    : _versionUrl(versionUrl), _firmwareUrl(firmwareUrl), _currentVersion(currentVersion) {}

bool OtaManager::isNewVersionAvailable(String& newVersion) {
    WiFiClientSecure client;
    client.setInsecure(); // ignora validação do certificado SSL

    HTTPClient http;
    http.begin(client, _versionUrl);
    int httpCode = http.GET();

    if (httpCode != HTTP_CODE_OK) {
        Serial.printf("[OTA] Falha ao verificar versão. HTTP: %d\n", httpCode);
        http.end();
        return false;
    }

    newVersion = http.getString();
    newVersion.trim();
    http.end();

    Serial.printf("[OTA] Versão atual: %s | Servidor: %s\n", _currentVersion, newVersion.c_str());
    return newVersion != String(_currentVersion);
}

void OtaManager::checkAndUpdate() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[OTA] Sem WiFi, pulando verificação.");
        return;
    }

    String newVersion;
    if (!isNewVersionAvailable(newVersion)) {
        Serial.println("[OTA] Firmware já está atualizado.");
        return;
    }

    Serial.printf("[OTA] Nova versão encontrada: %s. Iniciando atualização...\n", newVersion.c_str());

    httpUpdate.onStart([]() {
        Serial.println("[OTA] Iniciando download...");
    });
    httpUpdate.onProgress([](int current, int total) {
        Serial.printf("[OTA] Progresso: %d%%\r", (current * 100) / total);
    });
    httpUpdate.onEnd([]() {
        Serial.println("\n[OTA] Download concluído!");
    });
    httpUpdate.onError([](int err) {
        Serial.printf("\n[OTA] Erro: %s\n", httpUpdate.getLastErrorString().c_str());
    });

    WiFiClientSecure client;
    client.setInsecure(); // ignora validação do certificado SSL

    t_httpUpdate_return ret = httpUpdate.update(client, _firmwareUrl);

    switch (ret) {
        case HTTP_UPDATE_FAILED:
            Serial.printf("[OTA] Falha: (%d) %s\n",
                httpUpdate.getLastError(),
                httpUpdate.getLastErrorString().c_str());
            break;
        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("[OTA] Sem atualizações.");
            break;
        case HTTP_UPDATE_OK:
            Serial.println("[OTA] Atualização concluída! Reiniciando...");
            break;
    }
}