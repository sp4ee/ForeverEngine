#include "display_oled.h"
#include "engine_controller.h"
#include "coil.h"
#include "cycle_ix.h"
#include "magic.h"
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/io.h>
#include <Arduino.h>

#include "vcc.h"

volatile EngineController engine_controller;
Display display;

void setup()
{
    coil_init();
    //sensor_init();
    engine_controller.setup();
    display.setup();

    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
}

ISR(TIMER2_COMPA_vect)
{
    engine_controller.tick();
}

ISR(ANALOG_COMP_vect)
{
    engine_controller.comparator(ACSR & bit(ACO));
}

ISR(ADC_vect)
{
    engine_controller.adc_ready();
}

int16_t last_update_ix = 0;

void loop()
{
    int16_t ix = (millis() % INT16_MAX);
    int16_t since_last_update = sub_cycles(ix, last_update_ix);
    if (since_last_update > 500)
    {
        //int16_t vcc = get_vcc(); // 4876

        last_update_ix = ix;
        cli();
        int16_t x_rpm = engine_controller.rpm;
        int16_t x_duty = engine_controller.duty;
        int16_t x_x = engine_controller.hall_reading;
        sei();
        display.update(x_rpm, x_duty, x_x);
    }
    delay(50);
}
