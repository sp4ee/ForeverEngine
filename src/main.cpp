#include "engine_controller.h"
#include "coil.h"
#include "sensor.h"
#include "cycle_ix.h"
#include "magic.h"
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/io.h>
#include <Arduino.h>
#include <Tiny4kOLED.h>

#define BUFSIZE 64
char buf[BUFSIZE];
volatile EngineController engine_controller;

void setup()
{
    coil_init();
    sensor_init();
    engine_controller.setup();

    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);


    Wire.begin((int)400000);
    oled.begin(128, 32, sizeof(tiny4koled_init_128x32), tiny4koled_init_128x32);
    oled.on();
    oled.switchRenderFrame();
    oled.setContrast(0x10);
    oled.clear();
    oled.setFont(FONT6X8CAPS);
    oled.setCursor(0, 0);
    oled.print(F("FOREVER ENGINE"));
    oled.setCursor(0, 2);
    oled.print(F("STANDING BY"));
    oled.switchFrame();
    delay(100);
}

ISR(TIMER2_COMPA_vect)
{
    engine_controller.tick();
}

ISR(ANALOG_COMP_vect)
{
    engine_controller.comparator(ACSR & bit(ACO));
}

int16_t last_update_ix = 0;

void loop()
{
    int16_t ix = (millis() % INT16_MAX);
    int16_t since_last_update = sub_cycles(ix, last_update_ix);
    if (since_last_update > 500)
    {
        last_update_ix = ix;
        cli();
        int16_t x_rpm = engine_controller.rpm;
        int16_t x_control = engine_controller.duty;
        sei();
        oled.clear();
        snprintf(buf, BUFSIZE, "RPM: %d  C: %d", x_rpm, x_control);
        oled.setCursor(0, 0);
        oled.print(buf);
        oled.switchFrame();
    }
    delay(50);
}
