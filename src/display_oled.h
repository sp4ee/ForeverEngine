#pragma once
#include <Arduino.h>

#define DISPLAY_BUFSIZE 64

struct Display
{
    char buf[DISPLAY_BUFSIZE];

    void setup();
    void update(int16_t rpm, int16_t duty);
};
