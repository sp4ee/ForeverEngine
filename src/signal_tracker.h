#pragma once
#include <Arduino.h>
#include "magic.h"
#include "cycle_ix.h"

struct SignalTracker
{
    // Time index of last signal.
    int16_t last_signal_ix;
    
    // Delay of recorded signals (since previous one)
    int16_t kept_signal_delays[KEEP_N_SIGNALS];

    // Index of next signal delay to store
    uint8_t next_array_ix;

    // Rolling sum of signals recorded so far.
    int16_t rolling_sum;

    // Number of signals recorded so far.
    int16_t recorded_count;

    SignalTracker()
    {
        reset();
    }

    void reset()
    {
        memset(this, 0, sizeof(SignalTracker));
    }

    void record_signal(int16_t ix)
    {
        // Very first record: cannot calculate delay; just remember cycle index
        if (last_signal_ix == 0)
        {
            last_signal_ix = ix;
            return;
        }
        // Remove oldest value from rolling sum
        rolling_sum -= kept_signal_delays[next_array_ix];
        // Calculate delay
        int16_t delay = sub_cycles(ix, last_signal_ix);
        // Store in new slot in array
        kept_signal_delays[next_array_ix] = delay;
        // Update index
        ++next_array_ix;
        if (next_array_ix == KEEP_N_SIGNALS) next_array_ix = 0;
        // How many values have we seen?
        if (recorded_count != KEEP_N_SIGNALS) ++recorded_count;
        // This index becomes last signal index
        last_signal_ix = ix;
    }

    int16_t get_avg_delay()
    {
        if (recorded_count == 0) return 20;
        if (recorded_count != KEEP_N_SIGNALS) return rolling_sum / recorded_count;
        return rolling_sum / KEEP_N_SIGNALS;
    }

};
