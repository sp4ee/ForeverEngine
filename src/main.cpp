#include "magic.h"
#include "signal_tracker.h"
#include "cycle_ix.h"
#include <Arduino.h>

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
}

SignalTracker signal_tracker;
int16_t last_comp = 0;

void loop()
{
    uint16_t comp = ACSR & bit(ACO);
    if (comp != 0 && last_comp == 0)
    {
        int16_t ix = millis() % INT16_MAX;
        signal_tracker.record_signal(ix);
        int16_t avg_delay = signal_tracker.get_avg_delay();
        int16_t pulse_len_micros = avg_delay * 1000 / 400;
        coil_push();
        delayMicroseconds(pulse_len_micros);
        coil_off();
    }
    last_comp = comp;
    delay(1);
}
