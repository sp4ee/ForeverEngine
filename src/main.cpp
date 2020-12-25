#include "magic.h"
#include "signal_tracker.h"
#include "cycle_ix.h"
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
int16_t last_comp = 0;
int16_t last_update_ix = 0;
int16_t pulse_len_micros = 0;
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
           //| bit(ACIS1)  // ACIS1, ACIS0: Analog Comparator Interrupt Mode Select (trigger on falling edge)
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
}

ISR(ANALOG_COMP_vect)
{
 // Put your code here
}

void loop()
{
    int16_t ix = millis() % INT16_MAX;
    uint16_t comp = ACSR & bit(ACO);
    if (comp != 0 && last_comp == 0)
    {
        signal_tracker.record_signal(ix);
        int16_t avg_delay = signal_tracker.get_avg_delay();
        pulse_len_micros = avg_delay * 150 / 2;
        coil_push();
        delayMicroseconds(pulse_len_micros);
        coil_off();
    }
    last_comp = comp;

    int16_t since_last_update = sub_cycles(ix, last_update_ix);
    if (since_last_update > 500)
    {
    oled.clear();
        int16_t rolling_sum = signal_tracker.rolling_sum;
        snprintf(buf, BUFSIZE, "AD: %d  LPL: %d", rolling_sum / KEEP_N_SIGNALS, pulse_len_micros);
        oled.setCursor(0, 0);
        oled.print(buf);
        int16_t rpm = 30000 / rolling_sum;
        snprintf(buf, BUFSIZE, "RPM: %d", rpm);
        oled.setCursor(0, 2);
        oled.print(buf);
        oled.switchFrame();
        last_update_ix = ix;
    }

    delay(1);

}
