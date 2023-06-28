/*
Copyright (c) 2023-2023 Webtech Projekt
*/

#include "WifiManager.h"

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// Timer Functions
std::chrono::high_resolution_clock::time_point timeNow()
{
    return std::chrono::high_resolution_clock::now();
}

time_t getTimeNow()
{
    const auto now_c = std::chrono::system_clock::now();
    const auto now = std::chrono::system_clock::to_time_t(now_c);

    return now;
}

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

    // Time Config
    configTime(TZ_Europe_Berlin, "pool.ntp.org");
}

WifiManager::~WifiManager()
{

}

void WifiManager::Update(uint64_t difftime)
{
    if (timer <= 0)
    {
        // Connection Dropped
        if (!client.connected())
        {
            // Reconnect to our MQTT Broker
            reconnect();
            timer = 1 * TimeVar::Seconds;
        }
    }
    else
        timer -= difftime;

    // All Full 30 Seconds send a Value To our Server
    // Example when its 13:12:43 at startup the node will send
    // the first Message at 13:13:00 and continues all 30 sec.
    if ((getTimeNow() % 30) == 0 && !timeDone)
    {
        timeDone = true;
        // Send our Sensor Data to our MQTT Broker
        float temperature = precision(bme280.getTemperature(), 2);
        uint32_t pressure = bme280.getPressure();
        float altitude = precision(bme280.calcAltitude(pressure), 2);
        uint32_t humidity = bme280.getHumidity();
        long lux = precision(TSL2561.readVisibleLux(), 2);
        uint16_t sound = analogRead(PHOTOCELL_PIN);

        // Construct Json
        StaticJsonDocument<100> doc;
        char output[100];

        doc["temp"] = temperature;
        doc["pres"] = pressure;
        doc["alt"] = altitude;
        doc["hum"] = humidity;
        doc["lux"] = lux;
        doc["soun"] = sound;

        // Serialise
        serializeJson(doc, output);

        std::stringstream topic;
        topic << "Nodes/";
        topic << getClientId();
        topic << "/Data";

        sLogger.debug(output);
        client.publish(topic.str().c_str(), output);
    }
    else if ((getTimeNow() % 30) != 0)
        timeDone = false;

    client.loop();
}

std::string WifiManager::getClientId()
{
    // Get the wifi MAC-Address and convert it to a string
    std::string macAddress = WiFi.macAddress().c_str();

    // Convert the MAC-Address to lower case
    std::transform(macAddress.begin(), macAddress.end(), macAddress.begin(), ::tolower);

    // Remove the colons from the MAC-Address
    macAddress.erase(std::remove(macAddress.begin(), macAddress.end(), ':'), macAddress.end());

    // Return the formatted ClientId
    return macAddress;
}


void WifiManager::reconnect()
{
    sLogger.debug("MQTT not connected... Trying to connect");

    // Print our Node Id on the LCD
    lcd.print(getClientId().c_str());

    // Attempt an Connection
    // Login als Clients User um nur Berechtigte Clients anmelden zu können
    // Wenn der Login Erfolgreich war wird in der Datenbank die ClientId angelegt
    if (client.connect(getClientId().c_str(), "clients", "zbw"))
    {
        sLogger.debug("Connected to MQTT Broker");
        client.subscribe(acceptTopic);

        // Construct Json
        StaticJsonDocument<50> doc;
        char output[50];

        doc["id"] = getClientId();

        // Serialise
        serializeJson(doc, output);

        sLogger.debug(output);
        client.publish(acceptTopic, output);
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