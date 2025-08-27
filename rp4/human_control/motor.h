#ifndef __MOTOR_H__
#define __MOTOR_H__

void Go_Backward_Right(void);
void Go_Backward_Left(void);
void Go_Backward(void);
void Go_Forward_Right(void);
void Go_Forward_Left(void);
void Go_Forward(void);
void Turn_Left(void);
void Turn_Right(void);
void Motor_Stop(void);

int motor_init(void);
void motor_deinit(void);

#endif