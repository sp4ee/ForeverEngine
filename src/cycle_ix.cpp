#include "cycle_ix.h"

int cmp_cycles(int16_t cycle_a, int16_t cycle_b)
{
    if (cycle_a == cycle_b)
        return 0;
    int16_t diff = cycle_b - cycle_a;
    // B seems to be larger
    if (diff > 0)
    {
        // Larger by not too much: A < B
        if (diff < 0x6000)
            return -1;
        // Larger by a lot: we assume A has rolled over, so it is larger
        else
            return 1;
    }
    // A seems to be larger
    else
    {
        // Larger by not too much: A > B
        if (diff > -0x6000)
            return 1;
        // Larger by a lot: we assume B has rolled over, so it is larger
        else
            return -1;
    }
}

int16_t add_cycles(int16_t cycle_a, int16_t cycle_b)
{
    int16_t res = cycle_a + cycle_b;
    if (res < 0) res-= INT16_MIN;
    return res;
}

int16_t sub_cycles(int16_t later, int16_t older)
{
    if (later >= older) return later - older;
    else return INT16_MAX - older + later;
}