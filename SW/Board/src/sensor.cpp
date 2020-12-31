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
    ADCSRA = bit(ADEN); // turn ADC on
    // Prescaler 32: 38.4k conversions per second
    ADCSRA |= bit(ADPS0) | bit(ADPS2);
    // Prescaler of 1289.6k conversions per second
    //ADCSRA |= bit (ADPS0) |  bit (ADPS1) | bit (ADPS2);
    ADMUX = bit(REFS0) | (ADCPIN_HALLSENSOR & 0x07); // AVcc
}

void Sensor::adc_ready() volatile
{
    if (measuring_vcc)
        return;
    uint8_t low = ADCL;
    hall_reading = (ADCH << 8) | low;
    adc_working = 0;
    digitalWrite(PIN_HALL_SWITCH, LOW);
}

void Sensor::enable_hall_device() volatile
{
    digitalWrite(PIN_HALL_SWITCH, HIGH);
}

void Sensor::begin_adc() volatile
{
    ++adc_working;
    ADCSRA |= bit(ADSC) | bit(ADIE);
}

void Sensor::measure_vcc() volatile
{
    // Must check this value from critical section
    // Otherwise interrupt may call begin_adc() and we get hung up forever
    noInterrupts();
    adc_working += 1;
    uint8_t adc_working_copy = adc_working;
    if (adc_working_copy > 1) --adc_working;
    interrupts();
    if (adc_working_copy > 1) return;

    measuring_vcc = true;
    // VCC measurment magix
    // Reads internal 1V1 reference against VCC
#if defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny44__)
    ADMUX = bit(MUX5) | bit(MUX0); // For ATtiny84
#elif defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__)
    ADMUX = bit(MUX3) | bit(MUX2); // For ATtiny85/45
#elif defined(__AVR_ATmega1284P__)
    ADMUX = bit(REFS0) | bit(MUX4) | bit(MUX3) | bit(MUX2) | bit(MUX1); // For ATmega1284
#else
    ADMUX = bit(REFS0) | bit(MUX3) | bit(MUX2) | bit(MUX1); // For ATmega328
#endif
    delay(2);            // Wait for Vref to settle
    ADCSRA |= bit(ADSC); // Convert
    while (bit_is_set(ADCSRA, ADSC))
        ;
    uint8_t low = ADCL;
    int16_t reading = (ADCH << 8) | low;
    // Discard previous result
    ADCSRA |= bit(ADSC); // Convert
    while (bit_is_set(ADCSRA, ADSC))
        ;
    low = ADCL;
    reading = (ADCH << 8) | low;
    vcc = ((int32_t)1024 * 1100) / reading;
    // Configure system for measuring Hall sensor's voltage again.
    init_adc();
    measuring_vcc = false;
    adc_working = 0;
}
