#pragma once
#include "coil_reading_tracker.h"
#include "magic.h"
#include <Arduino.h>

// Manages ADC to retrieve reading of Hall sensor, and of supply voltage.
struct Sensor
{
    // 0 if ADC is currently not working.
    volatile uint8_t adc_working;
    // 1: supply voltage sync; 0: Hall sensor interrupt; 2: coil interrupt
    volatile uint8_t measure_mode;
    // Last converted Hall sensor value (10-bit).
    volatile int16_t hall_reading;
    // Last measured coil voltage (aggregated)
    volatile CoilReadingTracker coil_reading_tracker;
    // Last measured supply voltage (3300 = 3.3V).
    volatile int16_t vcc;

    Sensor();

    // Configures ADC at startup. Call from global setup().
    void setup() volatile;

    // Configures ADC for reading Hall sensor, or coild. Do not call from the outside.
    void init_adc(bool hall) volatile;

    // Handles ISR(ADC_vect).
    void adc_ready() volatile;

    // Turns power on/off for Hall sensor
    void set_hall_device_power(bool on) volatile;

    // Starts conversion of Hall sensor output or coil. Doesn't block. Only call from timer ISR.
    void begin_adc(bool read_hall) volatile;

    // Measures supply voltage. Blocks for > 1msec. Only call from outside ISR (i.e., from loop()).
    void measure_vcc() volatile;
};
