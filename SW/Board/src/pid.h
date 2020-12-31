#pragma once
#include <Arduino.h>

class PID
{
public:
    int16_t last_error = 0;
    int16_t integral_state = 0;

public:
    uint8_t update(int16_t error) volatile;
};
