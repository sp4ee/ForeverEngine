#pragma once

class Controller
{
public:
    double last_error = 0;
    double integral_state = 0;
    double integral_max = 20;
    double integral_min = -20;
    double ctrl_max = 40;
    double ctrl_min = 0;
    double kp = 0.075;
    double ki = 0.1;
    double kd = 8;

public:
    double update(double error);
};
