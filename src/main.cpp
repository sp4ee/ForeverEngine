#include "magic.h"
#include "signal_tracker.h"
#include "controller.h"
#include "cycle_ix.h"
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/io.h>
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
    digitalWrite(13, HIGH);
}

void coil_off()
{
    digitalWrite(14, LOW);
    digitalWrite(15, LOW);
    digitalWrite(13, LOW);
}

SignalTracker signal_tracker;
Controller controller;

#define BUFSIZE 64
char buf[BUFSIZE];

void setup()
{
    Serial.begin(115200);

    pinMode(4, INPUT_PULLUP);
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);

    pinMode(14, OUTPUT);
    pinMode(15, OUTPUT);
    coil_off();

    ADCSRB = 0;          // (Disable) ACME: Analog Comparator Multiplexer Enable
    ACSR = 0;
    // ACSR = bit(ACI)      // (Clear) Analog Comparator Interrupt Flag
    //        | bit(ACIE)   // Analog Comparator Interrupt Enable
    //        ;

    cli();
    TCCR2A = (1 << WGM21);
    TCCR2B = (1 << CS22) | (1 << CS21); // 256 prescale: 16 000 000 / 256 = 62500
    TCNT2  = 0;
    OCR2A = 6;            // compare match register: roughly 0.1 msec (because 62500 / 6 = 10416.67)
    TIMSK2 = (1 << OCIE2A);  // enable timer compare interrupt
    sei();

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

volatile int16_t turnoff_counter = 0;
volatile uint16_t control_counter = 0;
volatile int32_t rpm = 0;
volatile int16_t last_comp = 0;
volatile int16_t last_sensor = 0;
volatile int16_t control = 25;
volatile int32_t last_error = -1;

ISR(TIMER2_COMPA_vect)
{
    int16_t sensor = digitalRead(4);
    //digitalWrite(13, sensor);
    if (sensor == LOW && last_sensor == HIGH)
    {
        int16_t ix = millis() % INT16_MAX;
        signal_tracker.record_signal(ix);
    }
    last_sensor = sensor;

    uint16_t comp = ACSR & bit(ACO);
    // Edge right now, and coils is not activte RN
    if (comp != 0 && last_comp == 0 && turnoff_counter == 0 && control != 0)
    {
        turnoff_counter = control;
        //turnoff_counter = 25;
        coil_push();
    }
    last_comp = comp;

    if (turnoff_counter > 0)
    {
        --turnoff_counter;
        if (turnoff_counter == 0) coil_off();   
    }

    ++control_counter;
    if (control_counter == 100)
    {
        control_counter = 0;
        rpm = (int32_t)7500 * signal_tracker.recorded_count / signal_tracker.rolling_sum;
        last_error = (int32_t)TARGET_RPM - rpm;
        double duty = controller.update(last_error);
        control = (uint16_t)duty;
        //control = (uint8_t)((double)signal_tracker.rolling_sum * duty);
        //if (control > 25) control = 25;
        //if (control < 20) control = 20;
    }
}

ISR(ANALOG_COMP_vect)
{
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
        int16_t x_rpm = (int16_t)rpm;
        int16_t x_control = control;
        int16_t x_rs = signal_tracker.rolling_sum;
        int16_t x_toc = (int32_t)last_error;
        sei();
        oled.clear();
        snprintf(buf, BUFSIZE, "RPM: %d  RS: %d", x_rpm, x_rs);
        oled.setCursor(0, 0);
        oled.print(buf);
        snprintf(buf, BUFSIZE, "C: %d  TX: %d", x_control, x_toc);
        oled.setCursor(0, 2);
        oled.print(buf);
        oled.switchFrame();
    }
    delay(50);
}
