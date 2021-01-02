#include "pid.h"
#include "magic.h"

uint8_t PID::update(int16_t error) volatile
{
    // Calculate proportional term
    int16_t p_term = PID_GAIN_P * error; 
    // Calculate the integral state with appropriate limiting
    integral_state += error;
    if (integral_state > PID_INTEGRAL_LIMIT) integral_state = PID_INTEGRAL_LIMIT;
    else if (integral_state < -PID_INTEGRAL_LIMIT) integral_state = -PID_INTEGRAL_LIMIT;
    // Calculate the integral term
    int16_t integral_term = PID_GAIN_I * integral_state;
    // Calculate the derivative term, manage state
    int16_t deriv_term = PID_GAIN_D * (error - last_error);
    last_error = error;
    // Done
    int16_t duty = p_term + integral_term + deriv_term;
    if (duty < 0) duty = 0;
    duty /= 128;
    if (duty > PID_MAX_DUTY) return PID_MAX_DUTY;
    return (uint8_t)duty;
}
