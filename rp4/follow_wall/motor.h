#ifndef __MOTOR_H__
#define __MOTOR_H__

int init_motor(void);
void motor_stop(void);
void Go_Forward_Turn(float turn_bias);
void Go_Turn(float turn_bias);

#endif