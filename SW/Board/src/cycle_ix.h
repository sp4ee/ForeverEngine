#pragma once
#include <Arduino.h>

// Compares two cycle indexes. Returns -1 if A < B; 0 if equal; 1 if A > B.
// Uses heuristics to detect rollover of only one cycle. If A is near 0 and B is near INT16_MAX, then A > B.
int cmp_cycles(int16_t cycle_a, int16_t cycle_b);

// Adds two positive cycle values. Sum is rolled around if exceeds INT16_MAX.
int16_t add_cycles(int16_t cycle_a, int16_t cycle_b);

// Retrieves delay between two cycles. Correct even when counter has rolled over in later.
int16_t sub_cycles(int16_t later, int16_t older);
