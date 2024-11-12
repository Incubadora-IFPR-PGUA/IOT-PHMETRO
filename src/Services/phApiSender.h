#pragma once

#include <UrlEncode.h> 
#include <HTTPClient.h>

class phApiSender {
public:
    static void sendPhToApi(float phValue);
};