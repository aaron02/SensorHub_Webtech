/*
Copyright (c) 2023-2023 Webtech Projekt
*/

#include "WifiManager.h"

WiFiClient wifiClient;
PubSubClient client(wifiClient);

//Display Init
rgb_lcd lcd;

const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

//RGB
int rgb_R = 13;
int rgb_G = 15;
int rgb_B = 12;

//Temperatursensor
BME280 bme280;

//Definition Sound Sensor
#define PHOTOCELL_PIN A0

WifiManager::WifiManager()
{
    sLogger.debug("Connecting to: %s", ssid);

    // Begin WiFi
    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        sLogger.debug("WiFi Connection establishing....");
    }

    sLogger.debug("WiFi Connection Succesfully IP: %s", WiFi.localIP().toString().c_str());

    // Setup MQTT Server Details
    client.setServer(mqtt_serv, 1883);
    client.setCallback(callback);

    //Initial Temperatursensor
    if(!bme280.init())
        sLogger.debug("BME 280 Device Error!");
    
    //Initial Lichtsensor
    TSL2561.init();

    //Inital RGB
    pinMode(rgb_R, OUTPUT);
    pinMode(rgb_G, OUTPUT);
    pinMode(rgb_B, OUTPUT);

    digitalWrite(rgb_R, LOW);
    digitalWrite(rgb_G, LOW);
    digitalWrite(rgb_B, LOW);

    //LCD Setup
    lcd.begin(16, 2);
    lcd.setRGB(colorR, colorG, colorB);
    lcd.print("LCD Initialized");
}

WifiManager::~WifiManager()
{

}

void WifiManager::Update(uint64_t difftime)
{
    if (timer <= 0)
    {
        timer = 1 * TimeVar::Seconds;

        // Connection Dropped
        if (!client.connected())
        {
            // Reconnect to our MQTT Broker
            reconnect();
        }
        else
        {
            // Send our Sensor Data to our MQTT Broker
            float temperature = bme280.getTemperature();
            float pressure = bme280.getPressure();
            float altitude = bme280.calcAltitude(pressure);
            float humidity = bme280.getHumidity();
            long lux = TSL2561.readVisibleLux();
            long sound = analogRead(PHOTOCELL_PIN);

            // Construct Json
            StaticJsonDocument<80> doc;
            char output[80];

            doc["temp"] = temperature;
            doc["pres"] = pressure;
            doc["alt"] = altitude;
            doc["hum"] = humidity;
            doc["lux"] = lux;
            doc["soun"] = sound;

            // Serialise
            serializeJson(doc, output);
            client.publish("data/sensors", output);
        }
    }
    else
        timer -= difftime;

    client.loop();
}

void WifiManager::reconnect()
{
    sLogger.debug("MQTT not connected... Trying to connect");

    // ClientId
    clientId = "Node-";
    clientId += WiFi.macAddress().c_str();

    // Print our Node Id on the LCD
    lcd.print(clientId.c_str());

    // Attempt an Connection
    // Login als Clients User um nur Berechtigte Clients anmelden zu können
    // Wenn der Login Erfolgreich war wird in der Datenbank die ClientId angelegt
    if (client.connect(clientId.c_str(), "clients", "zbw"))
    {
        sLogger.debug("Connected to MQTT Broker");
        client.subscribe(acceptTopic);
    }
    else
    {
        sLogger.debug("Failed ErrorCode = %i new appempt in approx 1 second", client.state());
    }
}

void WifiManager::callback(char* topic, byte* payload, unsigned int length) 
{
    // Convert the payload to a string
    std::string payloadString(reinterpret_cast<char *>(payload), length);

    // Create MQTT message opbject
    MqttMessage mqttMessage = {std::string(topic), payloadString};
}