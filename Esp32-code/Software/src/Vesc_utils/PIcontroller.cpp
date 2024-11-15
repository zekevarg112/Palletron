#include "./PIcontroller.h"

PIController::PIController(float kp, float ki, float outputMin, float outputMax, float rateLimit)
    : kp_(kp), ki_(ki), outputMin_(outputMin), outputMax_(outputMax), rateLimit_(rateLimit),
        integral_(0.0), prevOutput_(0.0), prevTime_(0), setpoint_(0.0) {}

// Update the controller with the current error and timestep
float PIController::update(float measured) {
    if(setpoint_ == 0.0) return 0.0;
    uint32_t currentTime = millis();
    float error = setpoint_ - measured;
    uint32_t deltaTime = currentTime - prevTime_;
    //printf("PI measured: %f setpoint: %f error: %f\n", measured, setpoint_, error);

    if (deltaTime <= 0.0) {
        return prevOutput_;
    }

    // Proportional term
    float pTerm = kp_ * error;

    // Integral term with anti-windup
    integral_ += error * float(deltaTime);
    float iTerm = ki_ * integral_;

    // Calculate preliminary output
    float output = pTerm + iTerm;

    // Apply saturation (output limiting)
    if (output > outputMax_) {
        output = outputMax_;
        integral_ -= error * deltaTime;  // Anti-windup
    } else if (output < outputMin_) {
        output = outputMin_;
        integral_ -= error * deltaTime;  // Anti-windup
    }

    // Apply rate limiting
    float rate = (output - prevOutput_) / deltaTime;
    if (rate > rateLimit_) {
        output = prevOutput_ + rateLimit_ * deltaTime;
    } else if (rate < -rateLimit_) {
        output = prevOutput_ - rateLimit_ * deltaTime;
    }

    // Update for the next iteration
    prevOutput_ = output;
    prevTime_ = currentTime;

    return output;
}

void PIController::reset() {
    printf("PI controller reset!");
    integral_ = 0.0;
    prevOutput_ = 0.0;
    prevTime_ = 0;
}

void PIController::setpoint(float new_setpoint){
    if(new_setpoint <= 1000.0 && new_setpoint >= -1000.0) {
        setpoint_ = new_setpoint;
        printf("PI new setpoint %f\n", new_setpoint);
    }
}
