#include "Defnies.h"

long new_time = 0;
long old_time = 0;
int32_t timer = 1 * TimeVar::Seconds;
uint8_t status = Status::Startup;

WifiManager* mWifi = nullptr;

void MainThread(uint32_t difftime)
{
    // Update WifiManager
    CALL_UPDATE(mWifi, Update(difftime));

    // Test Timer 1 second
    if (0)
    {
        if (timer < 0)
        {
            sLogger.info("Controller Loop Time = %u µs (%f ms)", difftime, float(float(difftime) / Millis));
            timer = 10 * TimeVar::Seconds;
        }
        else
            timer = timer - difftime;
    }
}

void setup()
{
    // Serial Interface Initialization
    Serial.begin(9600);
    Wire.begin();
    delay(100);

    status = Status::Startup;

    // Notice us we are Starting now
    sLogger.info("Controller Staring....");

    // Set Debug Level for Logger
    sLogger.setLogType(MessageType::DEBUG);

    // Initialize Interfaces
    sLogger.info("Controller Initialize Interfaces....");
    
    mWifi = new WifiManager();

    //

    status = Status::Started;
    // Notice Our Logs we are Running :)
    sLogger.info("Controller Running....");
}

void loop()
{
    if (!status)
        return;

    new_time = micros();
    uint32_t difftime = new_time - old_time;

    MainThread(difftime);

    // End Loop
    old_time = new_time;
}