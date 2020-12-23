#include "magic.h"
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
    pinMode(4, INPUT);

    ADCSRB = 0;          // (Disable) ACME: Analog Comparator Multiplexer Enable
    ACSR = bit(ACI)      // (Clear) Analog Comparator Interrupt Flag
           | bit(ACIE)   // Analog Comparator Interrupt Enable
           //| bit(ACIS1)  // ACIS1, ACIS0: Analog Comparator Interrupt Mode Select (trigger on falling edge)
           ;
}

#define BUFSIZE 32
char buf[BUFSIZE];

int16_t last_sensor = HIGH;
int16_t last_comp = 0;

ISR(ANALOG_COMP_vect) { }

void loop()
{
    uint16_t comp = ACSR & bit(ACO);
    digitalWrite(13, comp != 0);
    if (comp != 0)
    {
        if (last_comp != comp)
        {
            coil_push();
            delay(1);
            coil_off();
        }
    }
    last_comp = comp;
    delay(5);

    // int16_t sensor = digitalRead(4);
    // if (sensor == LOW)
    // {
    //     digitalWrite(13, HIGH);
    //     if (last_sensor == HIGH)
    //     {
    //         coil_push();
    //         delay(2);
    //         coil_off();
    //     }
    // }
    // else
    //     digitalWrite(13, LOW);
    // last_sensor = sensor;
    // delay(10);
}
