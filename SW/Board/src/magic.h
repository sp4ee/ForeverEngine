#pragma once

#define TARGET_RPM              120
#define KEEP_N_SIGNALS          8
#define MAGNET_COUNT            8
#define PIN_COIL_A              14
#define PIN_COIL_B              15
#define PIN_HALL_SWITCH         3
#define ADCPIN_HALLSENSOR       2    // ADC2 is A2; would be Arduino pin 16 on Nano
#define HALLSENSOR_THRESHOLD    700
#define VCC_MEASURE_SECONDS     60

//#define PID_GAIN_P              13   // 8
//#define PID_GAIN_I              10   // 16
//#define PID_GAIN_D              128  // 256
#define PID_GAIN_P              64   // 8
#define PID_GAIN_I              96   // 16
#define PID_GAIN_D              4096  // 256
#define PID_INTEGRAL_LIMIT      96
#define PID_MAX_DUTY            96
