#include "sensor.h"

void Sensor::setup() volatile
{
    // Pin turning Hall sensor on/off
    pinMode(PIN_HALL_SWITCH, OUTPUT);
    digitalWrite(PIN_HALL_SWITCH, LOW);
    // Initialize ADC
    init_adc();
}

void Sensor::init_adc() volatile
{
    // http://www.gammon.com.au/adc
    ADCSRA =  bit (ADEN);   // turn ADC on
    // Prescaler 32: 38.4k conversions per second
    ADCSRA |= bit (ADPS0) | bit (ADPS2);
    // Prescaler of 1289.6k conversions per second
    //ADCSRA |= bit (ADPS0) |  bit (ADPS1) | bit (ADPS2);
    ADMUX =   bit (REFS0) | (ADCPIN_HALLSENSOR & 0x07);  // AVcc
}

void Sensor::adc_ready() volatile
{
    hall_reading = ADC;
    adc_working = false;
    digitalWrite(PIN_HALL_SWITCH, LOW);
}

void Sensor::enable_hall_device() volatile
{
    digitalWrite(PIN_HALL_SWITCH, HIGH);
}

void Sensor::begin_adc() volatile
{
    ADCSRA |= bit (ADSC) | bit (ADIE);
    adc_working = true;
}
