#include "display_oled.h"
#include "engine_controller.h"
#include "coil.h"
#include "sensor.h"
#include "src/Logic/cycle_ix.h"
#include "src/Logic/magic.h"
#include <Arduino.h>

volatile Sensor sensor;
volatile EngineController engine_controller(sensor);
Display display;

void setup()
{
    coil_init();
    sensor.setup();
    engine_controller.setup();
    display.setup();

    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);

    //coil_push();
}

ISR(TIMER2_COMPA_vect)
{
    engine_controller.tick();
}

ISR(ADC_vect)
{
    sensor.adc_ready();
}

int16_t last_update_ix = 0;
int16_t half_seconds = 0;

void loop()
{
    int16_t ix = (millis() % INT16_MAX);
    int16_t since_last_update = sub_cycles(ix, last_update_ix);
    if (since_last_update > 500)
    {
        last_update_ix = ix;

        ++half_seconds;
        if ((half_seconds / 2) % VCC_MEASURE_SECONDS == 0)
            sensor.measure_vcc();

        noInterrupts();
        int16_t x_rpm = engine_controller.rpm;
        int16_t x_duty = engine_controller.duty;
        int16_t x_x = (half_seconds % 2) + 1;
        interrupts();
        display.update(x_rpm, x_duty, x_x);
    }
    delay(50);
}
