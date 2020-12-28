#include "signal_tracker.h"
#include "cycle_ix.h"

#define RPM_TERM  (60000 / MAGNET_COUNT)

void SignalTracker::reset() volatile
{
    memset((void*)this, 0, sizeof(SignalTracker));
    last_signal_ix = -1;
}

bool SignalTracker::record_signal(int16_t ix) volatile
{
    // Very first record: cannot calculate delay; just remember cycle index
    if (last_signal_ix == -1)
    {
        last_signal_ix = ix;
        return false;
    }
    // Calculate delay
    int16_t delay = sub_cycles(ix, last_signal_ix);
    // After a long delay, reset. Engine was obviously stopped.
    if (delay > 255)
    {
        reset();
        return false;
    }
    // Remove oldest value from rolling sum
    rolling_sum -= kept_signal_delays[next_array_ix];
    // Add delay to rolling sum
    rolling_sum += (uint8_t)delay;
    // Store in new slot in array
    kept_signal_delays[next_array_ix] = (uint8_t)delay;
    // Update index
    ++next_array_ix;
    if (next_array_ix == KEEP_N_SIGNALS)
        next_array_ix = 0;
    // How many values have we seen?
    if (recorded_count != KEEP_N_SIGNALS)
        ++recorded_count;
    // This index becomes last signal index
    last_signal_ix = ix;
    // We've procesesd this signal.
    return true;
}

int16_t SignalTracker::calculate_rpm() volatile const
{
    if (recorded_count == KEEP_N_SIGNALS)
        return (int16_t)((int32_t)RPM_TERM * (int32_t)KEEP_N_SIGNALS / (int32_t)rolling_sum);
    else if (recorded_count != 0)
        return (int16_t)((int32_t)RPM_TERM * (int32_t)recorded_count / (int32_t)rolling_sum);
    else return 0;
}
