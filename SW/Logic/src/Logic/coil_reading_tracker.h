#pragma once
#include "magic.h"
#include <stdint.h>

struct CoilReadingTracker
{
    volatile int16_t readings[KEEP_N_COIL_READS];
    volatile uint8_t next_array_ix;
    volatile int16_t rolling_sum;

    CoilReadingTracker();

    inline void record_reading(int16_t reading) volatile
    {
        // Lower value: we assume coil is powered up, so reading is irrelevant.
        if (reading < COIL_FILTER_THRESHOLD) return;
        // Coil is on positive line; let's track how much _lower_ than reference (VCC) it is.
        reading = 1024 - reading;
        // Remove oldest value from rolling sum
        rolling_sum -= readings[next_array_ix];
        // Add value to rolling sum
        rolling_sum += reading;
        // Store in new slot in array
        readings[next_array_ix] = reading;
        // Update index
        ++next_array_ix;
        if (next_array_ix == KEEP_N_SIGNALS)
            next_array_ix = 0;
    }
};
