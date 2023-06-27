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

protected:
    // Timer
    int32_t timer = 1 * TimeVar::Seconds;

    // Wifi Settings
    const char* ssid = "";
    const char* pass = "";
    const char* mqtt_serv = "";

    std::string clientId = "unk";
    const char* acceptTopic = "client/accepted";
};