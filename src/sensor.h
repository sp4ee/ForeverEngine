#pragma once
#include <Arduino.h>
#include "magic.h"

inline void sensor_init()
{
    pinMode(PIN_SENSOR, INPUT_PULLUP);
}

inline bool sensor_read()
{
    bool res = digitalRead(PIN_SENSOR) == LOW;
    //digitalWrite(13, res ? HIGH : LOW);
    return res;
}
