#pragma once
#include <Arduino.h>
#include "magic.h"

struct Sensor
{
    volatile uint8_t adc_working = 0;
    volatile bool measuring_vcc = false;
    volatile int16_t hall_reading = 0;
    volatile int16_t vcc = 0;

    void setup() volatile;

    void init_adc() volatile;

    void adc_ready() volatile;

    void enable_hall_device() volatile;

    void begin_adc() volatile;

    void measure_vcc() volatile;
};
