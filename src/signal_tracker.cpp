#include "signal_tracker.h"

bool SignalTracker::record_signal(int16_t ix)
{
    // Very first record: cannot calculate delay; just remember cycle index
    if (last_signal_ix == -1)
    {
        last_signal_ix = ix;
        return false;
    }
    // Calculate delay
    int16_t delay = sub_cycles(ix, last_signal_ix);
    // If delay is weird, ignore (maybe reset)
    if (delay > 500)
    {
        reset();
        return false;
    }
    last_delay = delay;
    // Remove oldest value from rolling sum
    rolling_sum -= kept_signal_delays[next_array_ix];
    // Add delay to rolling sum
    rolling_sum += last_delay;
    // Store in new slot in array
    kept_signal_delays[next_array_ix] = last_delay;
    // Update index
    ++next_array_ix;
    if (next_array_ix == KEEP_N_SIGNALS)
        next_array_ix = 0;
    // How many values have we seen?
    if (recorded_count != KEEP_N_SIGNALS)
        ++recorded_count;
    // This index becomes last signal index
    prev_signal_ix = last_signal_ix;
    last_signal_ix = ix;
    // We've procesesd this signal.
    return true;
}
