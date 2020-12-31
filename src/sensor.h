#pragma once
#include <Arduino.h>
#include "magic.h"

// Manages ADC to retrieve reading of Hall sensor, and of supply voltage.
struct Sensor
{
    // 0 if ADC is currently not working.
    volatile uint8_t adc_working = 0;
    // If true, current conversion is supply voltage; interrupt handler will not store value in hall_reading.
    volatile bool measuring_vcc = false;
    // Last converted Hall sensor value (10-bit).
    volatile int16_t hall_reading = 0;
    // Last measured supply voltage (3300 = 3.3V).
    volatile int16_t vcc = 0;

    // Configures ADC at startup. Call from global setup().
    void setup() volatile;

    // Configures ADC for reading Hall sensor. Do not call from the outside.
    void init_adc() volatile;

    // Handles ISR(ADC_vect).
    void adc_ready() volatile;

    // Turns on voltage for Hall sensor
    void enable_hall_device() volatile;

    // Starts conversion of Hall sensor output. Doesn't block. Only call from timer ISR.
    void begin_adc() volatile;

    // Measures supply voltage. Blocks for > 1msec. Only call from outside ISR (i.e., from loop()).
    void measure_vcc() volatile;
};
