#pragma once
#include <Arduino.h>
#include "magic.h"

struct Sensor
{
    volatile bool adc_working = false;
    volatile int16_t hall_reading = 0;

    void setup() volatile;

    void init_adc() volatile;

    void adc_ready() volatile;

    void enable_hall_device() volatile;

    void begin_adc() volatile;
};
