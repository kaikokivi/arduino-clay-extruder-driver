/*
 Stepper Driver Control Lib

 This program drives a stepper through a driver.
 The driver is attached to digital pins 8 and 9 and 3.3V of the Arduino.
 
 Created 28 May. 2020
 Modified 19 June 2025
 by Kaiko Kivi

 */

class Stepper
{
public:
    Stepper(int stepsPerRevolution, int step_pin, int dir_pin);
    Stepper(int stepsPerRevolution, int step_pin, int dir_pin, bool dir_invert = false);

    long speed;
    void setSpeed(long whatSpeed);
    void setMove(int steps_to_move);
    void setMove(int steps_to_move, long whatSpeed);
    void setMove(int steps_to_move, long whatSpeed, bool finish);
    int step();
    int stepDelay();
    unsigned long last_step_time; // time stamp in us of when the last step was taken
    unsigned long blink_time; // time stamp in us of when the last step was taken

private:
    void dirMotor(int dir);
    void stepMotor();
    int dir;
    int step_pin;
    int dir_pin;
    bool dir_invert; // if true, the motor is reversed
    int step_number;
    int steps_left;
    bool move_continue;
    unsigned long step_delay;
    int number_of_steps;
};
