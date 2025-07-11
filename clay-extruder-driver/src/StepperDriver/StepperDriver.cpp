/*
 Stepper Driver Control Lib

 This program drives a stepper through a driver.
 The driver is attached to digital pins 8 and 9 and 3.3V of the Arduino.

 Created 28 May. 2020
 Modified 19 June 2025
 by Kaiko Kivi

 */

 #include <Arduino.h>
#include "StepperDriver.h"

Stepper::Stepper(int stepsPerRevolution, int step_pin, int dir_pin)
{
    this->last_step_time = 0;
    this->blink_time = 0;
    this->number_of_steps = stepsPerRevolution;
    this->step_pin = step_pin;
    this->dir_pin = dir_pin;
    this->step_delay = 0;
    this->speed = 0;
    this->dir_invert = false; // default direction is not inverted
    pinMode(this->step_pin, OUTPUT);
    pinMode(this->dir_pin, OUTPUT);
}
Stepper::Stepper(int stepsPerRevolution, int step_pin, int dir_pin, bool dir_invert)
{
    this->last_step_time = 0;
    this->blink_time = 0;
    this->number_of_steps = stepsPerRevolution;
    this->step_pin = step_pin;
    this->dir_pin = dir_pin;
    this->step_delay = 0;
    this->speed = 0;
    this->dir_invert = dir_invert;
    pinMode(this->step_pin, OUTPUT);
    pinMode(this->dir_pin, OUTPUT);
}

void Stepper::setSpeed(long whatSpeed)
{
    this->speed = whatSpeed;
    if(this->speed == 0) {
        this->move_continue = false;
        digitalWrite(LED_BUILTIN, HIGH);
    }
    //Serial.println("setSpeed");
    this->step_delay = 60L * 1000L * 1000L / this->number_of_steps / abs(whatSpeed);
    // Serial.println(this->step_delay);
    if (whatSpeed > 0)
    {
        dirMotor(dir_invert ? 1 : 0); // if reverse is true, set direction to 0, else to 1
    }
    if (whatSpeed < 0)
    {
        dirMotor(dir_invert ? 0 : 1);
    }
}
void Stepper::setMove(int steps_to_move)
{
    this->steps_left = abs(steps_to_move); // how many steps to take
    this->last_step_time = micros();       // reset last_step_time to now
}
void Stepper::setMove(int steps_to_move, long whatSpeed)
{
    if(steps_to_move == 0 && whatSpeed != 0) {
        this->move_continue = true;
    } else {
        this->move_continue = false;
    }
    if(abs(whatSpeed) > 0) {
        this->setSpeed(whatSpeed);
        this->setMove(steps_to_move);
    } else {
        this->steps_left = 0;
    }
}
void Stepper::setMove(int steps_to_move, long whatSpeed, bool finish)
{
    this->setMove(steps_to_move, whatSpeed);
    if (finish)
        while (this->stepDelay())
        {
            delay(1);
        };
}
int Stepper::step()
{
    if (
        this->move_continue && // is in move
        this->step_delay != 0 && // speed is defined
        ( micros() - this->last_step_time) > this->step_delay
    ) // time to make a step has come
    {
        // Serial.println("step");
        stepMotor();
        this->last_step_time = this->last_step_time + this->step_delay;
        // Serial.print("Step time: ");
        // Serial.println(this->last_step_time);
        // above would fail to catch up if the motor is behind a more than a step
    }
    return this->last_step_time;
}
int Stepper::stepDelay()
{
    if (this->steps_left < 1 && !this->move_continue)
        return 0;
    unsigned long now = micros();
    while ((now - this->last_step_time) < this->step_delay)
    {
        now = micros();
    }
    // increment or decrement the step number,
    // depending on direction:
    if (this->dir == 1)
    {
        this->step_number++;
        if (this->step_number == this->number_of_steps)
        {
            this->step_number = 0;
        }
    }
    else
    {
        if (this->step_number == 0)
        {
            this->step_number = this->number_of_steps;
        }
        this->step_number--;
    }
    // decrement the steps left:
    this->steps_left--;
    // step the motor to step number 0, 1, ..., {3 or 10}
    stepMotor();
    this->last_step_time = this->last_step_time + this->step_delay;
    return this->steps_left;
}

void Stepper::dirMotor(int dir)
{
    if (dir == 1 && digitalRead(this->dir_pin) != HIGH)
        digitalWrite(this->dir_pin, HIGH);
    if (dir == 0 && digitalRead(this->dir_pin) != LOW)
        digitalWrite(this->dir_pin, LOW);
}
void Stepper::stepMotor()
{
    digitalWrite(this->step_pin, HIGH);

    delayMicroseconds(50);

    digitalWrite(this->step_pin, LOW);

    delayMicroseconds(50);

    if(
        ( micros() - this->blink_time) > this->step_delay * 800
    ) {
        digitalWrite(LED_BUILTIN, LOW);
        if(( micros() - this->blink_time) > this->step_delay * 1000) {
            digitalWrite(LED_BUILTIN, HIGH);
            this->blink_time = micros();
        }
    }
}
