#ifndef PI_CONTROLLER_H
#define PI_CONTROLLER_H

#include <Arduino.h>

class PIController {
public:
    PIController(float kp, float ki, float outputMin, float outputMax, float rateLimit);

    // Update the controller with the current error and timestep
    float update(float measured);

    void setpoint(float new_setpoint);

    void reset();

private:
    float setpoint_;
    float kp_;
    float ki_;
    float outputMin_;
    float outputMax_;
    float rateLimit_;
    
    float integral_;
    float prevOutput_;
    uint32_t prevTime_;
};

#endif