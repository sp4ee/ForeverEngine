#pragma once
#include "src/Logic/magic.h"
#include <Arduino.h>

inline void coil_off()
{
    digitalWrite(2, LOW);
}

inline void coil_init()
{
    pinMode(2, OUTPUT);
    coil_off();
}

inline void coil_pull()
{
}

inline void coil_push()
{
    digitalWrite(2, HIGH);
}

