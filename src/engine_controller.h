#pragma once
#include <Arduino.h>
#include "signal_tracker.h"
#include "pid.h"

struct EngineController
{
    volatile SignalTracker signal_tracker;
    volatile PID pid;
    volatile uint8_t turnoff_counter = 0;
    volatile uint16_t control_counter = 0;
    volatile bool last_sensor = false;
    volatile uint8_t last_comp = 0;
    volatile int16_t rpm = 0;
    volatile uint8_t duty = 25;

    void setup() volatile;

    void tick() volatile;

    void comparator(uint8_t comp) volatile;
};
