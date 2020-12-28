#include "controller.h"
#include <Arduino.h>

double Controller::update(double error)
{
    // Calculate proportional term
    double p_term = kp * error; 
    // Calculate the integral state with appropriate limiting
    integral_state += error;
    if (integral_state > integral_max) integral_state = integral_max;
    else if (integral_state < integral_min) integral_state = integral_min;
    // Calculate the integral term
    double integral_term = ki * integral_state;
    // Calculate the derivative term, manage state
    double deriv_term = kd * (error - last_error);
    last_error = error;
    // Done
    double ctrl = p_term + integral_term + deriv_term;
    if (ctrl < ctrl_min) return ctrl_min;
    if (ctrl > ctrl_max) return ctrl_max;
    return ctrl;
}
