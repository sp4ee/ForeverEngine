#include "magic.h"
#include "signal_tracker.h"
#include "controller.h"
#include "cycle_ix.h"
#include <avr/sleep.h>
#include <avr/power.h>
#include <Arduino.h>
#include <Tiny4kOLED.h>

void coil_pull()
{
    digitalWrite(14, LOW);
    digitalWrite(15, HIGH);
}

void coil_push()
{
    digitalWrite(14, HIGH);
    digitalWrite(15, LOW);
}

void coil_off()
{
    digitalWrite(14, LOW);
    digitalWrite(15, LOW);
}

SignalTracker signal_tracker;
Controller controller(TARGET_RPM);
int16_t last_comp = 0;
int16_t last_update_ix = 0;
int16_t last_control_ix = 0;
int16_t pulse_len_micros = 0;
int16_t rolling_sum = 0;
int16_t recorded_count = 0;
int32_t control = 55;

#define BUFSIZE 64
char buf[BUFSIZE];

void setup()
{
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);

    pinMode(14, OUTPUT);
    pinMode(15, OUTPUT);
    coil_off();

    ADCSRB = 0;          // (Disable) ACME: Analog Comparator Multiplexer Enable
    ACSR = bit(ACI)      // (Clear) Analog Comparator Interrupt Flag
           | bit(ACIE)   // Analog Comparator Interrupt Enable
           ;

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

ISR(ANALOG_COMP_vect)
{
}

void go_to_sleep()
{
    sleep_enable();
    set_sleep_mode(SLEEP_MODE_IDLE);
    power_adc_disable(); 
    power_usart0_disable();
    power_spi_disable(); 
    power_timer1_disable(); 
    power_timer2_disable();
    power_twi_disable();
    sleep_cpu();
    // Haha! Back.
    sleep_disable();
    power_all_enable();
}

void loop()
{
    //go_to_sleep();
    delay(1);
    int16_t ix = millis() % INT16_MAX;
    uint16_t comp = ACSR & bit(ACO);
    if (comp != 0 && last_comp == 0)
    {
        signal_tracker.record_signal(ix);
        rolling_sum = signal_tracker.rolling_sum;
        recorded_count = signal_tracker.recorded_count;
        pulse_len_micros = (uint16_t)(control * rolling_sum / recorded_count);
        coil_push();
        delayMicroseconds(pulse_len_micros);
        coil_off();
    }
    last_comp = comp;

    int16_t since_last_control = sub_cycles(ix, last_control_ix);
    if (since_last_control > 100)
    {
        int32_t rpm = (int32_t)7500 * recorded_count / rolling_sum;
        double newcontrol = controller.update(rpm);
        control = (int16_t)(newcontrol * 100.0);
        if (control < 0) control = 0;
        last_control_ix = ix;

        oled.clear();
        snprintf(buf, BUFSIZE, "AD: %d  LPL: %d", rolling_sum / recorded_count, pulse_len_micros);
        oled.setCursor(0, 0);
        oled.print(buf);
        //int32_t rpm = (int32_t)7500 * recorded_count / rolling_sum;
        snprintf(buf, BUFSIZE, "RPM: %d  C: %d", (int16_t)rpm, control);
        oled.setCursor(0, 2);
        oled.print(buf);
        oled.switchFrame();
    }

    int16_t since_last_update = sub_cycles(ix, last_update_ix);
    if (since_last_update > 500)
    {
        last_update_ix = ix;
    }
}
