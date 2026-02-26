#pragma once
#include <Arduino.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFi.h>

class OtaManager {
public:
    OtaManager(const char* versionUrl, const char* firmwareUrl, const char* currentVersion);
    void checkAndUpdate();

private:
    const char* _versionUrl;
    const char* _firmwareUrl;
    const char* _currentVersion;

    bool isNewVersionAvailable(String& newVersion);
};