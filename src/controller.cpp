#include "controller.h"
#include <Arduino.h>

Controller::Controller(double target_value)
: target_value(target_value)
{
    last_time = millis();
}

double Controller::update(double value)
{
    // Time elapsed since last update
    long curr_time = millis();
    double delta_time = (curr_time - last_time);
    // Update working error variables
    error = target_value - value;
    integral_term += ki * error * delta_time;
    if (integral_term > max_control) integral_term = max_control;
    else if (integral_term < -max_control) integral_term = -max_control;
    if (integral_term != integral_term) integral_term = 0;
    integral_term = 0;
    error_deriv = (error - last_error) / delta_time;
    // Remember for next time
    last_value = value;
    last_error = error;
    last_time = curr_time;
    // Compute PID result
    double control = kp * error + integral_term + kd * error_deriv;
    if (control > max_control) control = max_control;
    else if (control < -max_control) control = -max_control;

    last_control_deriv = (control - last_control) / delta_time;
    last_control = control;

    return control;
}
