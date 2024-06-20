#include "motor_controller.hpp"

MotorController::MotorController()
{
}

MotorController::~MotorController()
{
}

void MotorController::init()
{
    pinMode(md_pin.MOTOR_PWM, OUTPUT);
    pinMode(md_pin.MOTOR_IN1, OUTPUT);
    pinMode(md_pin.MOTOR_IN2, OUTPUT);
    pinMode(md_pin.MOTOR_EN, OUTPUT);
    digitalWrite(md_pin.MOTOR_EN, HIGH);
}
