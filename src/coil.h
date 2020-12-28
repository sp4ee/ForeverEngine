#pragma once
#include <Arduino.h>
#include "magic.h"

inline void coil_off()
{
    digitalWrite(PIN_COIL_A, LOW);
    digitalWrite(PIN_COIL_B, LOW);
}

inline void coil_init()
{
    pinMode(14, OUTPUT);
    pinMode(15, OUTPUT);
    coil_off();
}

inline void coil_pull()
{
    digitalWrite(PIN_COIL_A, LOW);
    digitalWrite(PIN_COIL_B, HIGH);
}

inline void coil_push()
{
    digitalWrite(PIN_COIL_A, HIGH);
    digitalWrite(PIN_COIL_B, LOW);
}

