#pragma once

class Controller
{
public:
    long last_time = 0;
    double error = 0;
    double error_deriv = 0;
    double last_value = 0;
    double last_error = 0;
    double target_value;
    double integral_term = 0;
    double kp = 0.1;
    double kd = 0.0;
    double ki = 0;
    double max_control = 1.0;
    double last_control = 0;
    double last_control_deriv = 0;
    bool enabled = true;

public:
    Controller(double target_value);
    double update(double value);
};
