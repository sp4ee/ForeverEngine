#pragma once
#include "magic.h"
#include <stdint.h>

struct SignalTracker
{
    // Time index of last signal.
    int16_t last_signal_ix;
    
    // Delay of recorded signals (since previous one)
    uint8_t kept_signal_delays[KEEP_N_SIGNALS];

    // Index of next signal delay to store
    uint8_t next_array_ix;

    // Rolling sum of signals recorded so far.
    uint16_t rolling_sum;

    // Number of signals recorded so far.
    int8_t recorded_count;

    SignalTracker()
    {
        reset();
    }

    void reset() volatile;

    bool record_signal(int16_t ix) volatile;

    int16_t calculate_rpm() volatile const;
};
