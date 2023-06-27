/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

enum Status
{
    Startup = 0,
    Started
};

#define CALL_UPDATE(obj, func) if (obj) static_cast<Updateable*>(obj)->func

// Arduino
#include <Arduino.h>

// Libaries
#include <Wire.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

// STD LIBS
#include <stdio.h>
#include <chrono>
#include <iostream>
#include <string>
#include <cstdarg>
#include <sstream> 
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <vector>
#include <array>
#include <cassert>
#include <string_view>

// User Created
#include "Updateable.h"
#include "Logger.h"
#include "TimeVar.h"
#include "WifiManager.h"

// Projekt Spezifisch
#include "Seeed_BME280.h"
#include <Digital_Light_TSL2561.h>
#include "rgb_lcd.h"
