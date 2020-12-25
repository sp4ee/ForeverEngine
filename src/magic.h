#pragma once

// Minimum voltage to exit power saving mode and get active (winking)
#define VCC_THRESHOLD_ACTIVE    3000

// Low cutoff voltage to exit winking and enter power saving (ie we have hysteresis)
#define VCC_THRESHOLD_SLEEP     2666

// Pin to which solenoid is connected
#define PIN_COIL                3

#define KEEP_N_SIGNALS          4

// 
#define PIN_LED                 13
