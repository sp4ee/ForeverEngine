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
    // Turn off coil if duty is over
    if (turnoff_counter != 0)
    {
        --turnoff_counter;
        if (turnoff_counter == 0)
            coil_off();
    }

    // ADC currently off: take a new measurement; alternate between Hall sensor and coil
    // If last one was *not* Hall, fire up sensor now.
    // We'll start conversion at the end of this tick().
    if (sensor.adc_working == 0 && sensor.measure_mode != 0)
        sensor.enable_hall_device();

    // Check last seen Hall sensor value; detect "signal", i.e., magnet passing.
    bool is_signal = sensor.hall_reading > HALLSENSOR_THRESHOLD;
    if (is_signal && !last_signal)
    {
        int16_t ix = millis() % INT16_MAX;
        signal_tracker.record_signal(ix);

        if (turnoff_counter == 0 && duty != 0 && rpm <= 64)
        {
            if (rpm > 30) turnoff_counter = 400;
            else turnoff_counter = 800;
            coil_push();
            idle_counter = 0;
        }
    }
    last_signal = is_signal;

    bool is_coil_induced = sensor.coil_rolling_sum > 20 * KEEP_N_COIL_READS;
    if (is_coil_induced && !last_coil_induced)
    {
        if (turnoff_counter == 0 && duty != 0 && rpm > 64)
        {
            turnoff_counter = duty;
            coil_push();
            idle_counter = 0;
        }
    }
    last_coil_induced = is_coil_induced;
    //digitalWrite(13, is_coil_induced);

    // Periodically, update PID calculation to get new duty value.
    ++control_counter;
    if (control_counter == 500)
    {
        control_counter = 0;
        rpm = signal_tracker.calculate_rpm();
        int16_t error = TARGET_RPM - rpm;
        duty = pid.update(error);
    }

    ++idle_counter;
    // Every 32768 ticks (ca 3 seconds): try a nudge
    if (idle_counter % 32768 == 0)
    {
        // TO-DO: Implement pull, when HW is there.
        coil_push();
        turnoff_counter = 800;
        kickstart_count = 2;
    }

    // Kick off ADC conversion if needed
    // If last wasn't Hall, measure Hall now; otherwise, coil.
    if (sensor.adc_working == 0) sensor.begin_adc(sensor.measure_mode != 0);
}

