/*
Copyright (c) 2023-2023 Webtech Projekt
*/

#pragma once

#include "Defnies.h"

struct MqttMessage
{
    std::string topic;
    std::string payload;
};

class WifiManager : public Updateable
{
public:
    WifiManager();
    ~WifiManager();

    // Cyclyc Update
    void Update(uint64_t difftime) override;

    void reconnect();

    static void callback(char* topic, byte* payload, unsigned int length);

    std::string getClientId();

private:
    // Helper functions
    float precision( float f, int places ) { float n = std::pow(10.0f, places ); return std::round(f * n) / n ; }

    bool timeDone = false;

protected:
    // Timer
    int32_t timer = 1 * TimeVar::Seconds;

    // Wifi Settings
    const char* ssid = "AndroidAP3853";
    const char* pass = "1234567890";
    const char* mqtt_serv = "188.172.228.39";

    std::string clientId = "unk";
    const char* acceptTopic = "client/accepted";
};