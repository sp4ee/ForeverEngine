#include "engine_controller.h"
#include "coil.h"
//#include "sensor_relay.h"

void EngineController::setup() volatile
{
    // Pin turning Hall sensor on/off
    pinMode(PIN_HALL_SWITCH, OUTPUT);
    digitalWrite(PIN_HALL_SWITCH, HIGH);

    // Set up analaog comparator (use coild to detect magnet passing)
    // (Disable) ACME: Analog Comparator Multiplexer Enable
    ADCSRB = 0;          
    // Analog Comparator Interrupt Enable
    ACSR = bit(ACI) | bit(ACIE);

    // Set up analog digital converter for Hall sensor output
    init_adc();

    // Set up 0.1ms timer - our main driver
    cli();
    // CTC mode (not PWM or some such thing)
    TCCR2A = (1 << WGM21);
    // 256 prescale: 16 000 000 / 256 = 62500
    TCCR2B = (1 << CS22) | (1 << CS21);
    TCNT2  = 0;
    // Compare match register: roughly 0.1 msec (because 62500 / 6 = 10416.67)
    OCR2A = 6;
    // Enable timer compare interrupt
    TIMSK2 = (1 << OCIE2A);
    sei();
}

void EngineController::init_adc() volatile
{
    // http://www.gammon.com.au/adc
    ADCSRA =  bit (ADEN);   // turn ADC on
    ADCSRA |= bit (ADPS0) |  bit (ADPS1) | bit (ADPS2);  // Prescaler of 128 (sampling frequency)
    ADMUX =   bit (REFS0) | (ADCPIN_HALLSENSOR & 0x07);  // AVcc
}

void EngineController::adc_ready() volatile
{
    hall_reading = ADC;
    adc_working = false;
}

void EngineController::tick() volatile
{
    // Currently not converting: start new conversion
    if (!adc_working)
    {
        ADCSRA |= bit (ADSC) | bit (ADIE);
        adc_working = true;
    }
    digitalWrite(13, hall_reading > 700);
    //bool sensor = sensor_read();
    bool sensor = hall_reading > 700;
    if (sensor && !last_sensor)
    {
        int16_t ix = millis() % INT16_MAX;
        signal_tracker.record_signal(ix);
        idle_counter = 0;
    }
    last_sensor = sensor;


    if (turnoff_counter != 0)
    {
        --turnoff_counter;
        if (turnoff_counter == 0)
            coil_off();   
    }

    ++control_counter;
    if (control_counter == 500)
    {
        control_counter = 0;
        rpm = signal_tracker.calculate_rpm();
        int16_t error = TARGET_RPM - rpm;
        duty = pid.update(error);
    }

    // ++idle_counter;
    // // Every 32768 ticks (ca 3 seconds): try a nudge
    // if (idle_counter % 32768 == 0)
    // {
    //     // Every third is a pull
    //     if (idle_counter % (3 * 32768) == 0)
    //         coil_pull();
    //     // Before that, it's a push
    //     else
    //         coil_push();
    //     turnoff_counter = 190;
    //     kickstart_count = 2;
    // }
}

void EngineController::comparator(uint8_t comp) volatile
{
    // Condition when we activate the coil at an edge:
    // Comparator output is high, and was not high before
    // Coil is not active right now (turnoff counter is 0)
    if (comp != 0 && last_comp == 0)
    {
        // It's a kickstart: first few pulses are independent of duty
        if (kickstart_count > 0)
        {
            turnoff_counter = 190;
            duty = 0;
            --kickstart_count;
            coil_push();
        }
        // Regular operation
        else if (turnoff_counter == 0 && duty != 0)
        {
            turnoff_counter = duty;
            coil_push();
        }
    }
    last_comp = comp;
}

