#include "engine_controller.h"
#include "coil.h"
#include "sensor.h"

EngineController::EngineController(volatile Sensor &sensor)
: sensor(sensor)
{
}

void EngineController::setup() volatile
{
    // Measure initial voltage so we can interpret Hall sensor reading correctly.
    sensor.measure_vcc();

    // Set up analaog comparator (use coild to detect magnet passing)
    // (Disable) ACME: Analog Comparator Multiplexer Enable
    ADCSRB = 0;          
    // Analog Comparator Interrupt Enable
    ACSR = bit(ACI) | bit(ACIE);

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

void EngineController::tick() volatile
{
    // ADC currently off: power up Hall sensor
    // At end of function, once some time has passed, we'll kick off ADC conversion
    if (sensor.adc_working == 0) sensor.enable_hall_device();
    // Check last seen Hall sensor value; detect "signal", i.e., magnet passing.
    bool is_signal = sensor.hall_reading > HALLSENSOR_THRESHOLD;
    if (is_signal && !last_signal)
    {
        int16_t ix = millis() % INT16_MAX;
        signal_tracker.record_signal(ix);
        idle_counter = 0;
    }
    last_signal = is_signal;

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
    
    // Kick off ADC conversion if needed
    if (sensor.adc_working == 0) sensor.begin_adc();
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

