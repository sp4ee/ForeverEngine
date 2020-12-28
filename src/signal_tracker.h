#pragma once
#include <Arduino.h>
#include "magic.h"
#include "cycle_ix.h"

struct SignalTracker
{
    // Time index of last signal.
    volatile int16_t last_signal_ix;
    
    // Delay of recorded signals (since previous one)
    volatile int16_t kept_signal_delays[KEEP_N_SIGNALS];

    // Index of next signal delay to store
    volatile uint8_t next_array_ix;

    // Rolling sum of signals recorded so far.
    volatile int32_t rolling_sum;

    // Number of signals recorded so far.
    volatile int16_t recorded_count;

    volatile int16_t prev_signal_ix;

    volatile int16_t last_delay;

    SignalTracker()
    {
        reset();
    }

    void reset()
    {
        memset(this, 0, sizeof(SignalTracker));
        last_signal_ix = -1;
    }

    bool record_signal(int16_t ix);

};
