#include <pigpio.h>
#include <stdio.h>
#include <gpiod.h>
#include <chrono>
#include <thread>
#include "motor.h"

#define GPIO_CHIP "/dev/gpiochip0"

#define PWM_FREQ    (1000)//HZ
#define PWM_DUTY_MIN    (30)
#define PWM_DUTY_AVG    (40)
#define PWM_DUTY_MAX    (50)

#define PIN_12EN    (26)
#define PIN_34EN    (16)
#define PIN_1A      (19)
#define PIN_2A      (18)
#define PIN_3A      (13)
#define PIN_4A      (12)


gpiod_chip *chip = NULL;
gpiod_line *Pin12EN_out = NULL;
gpiod_line *Pin34EN_out = NULL;

int init_gpio(void) {
    chip = gpiod_chip_open(GPIO_CHIP);
    if (!chip) { 
        printf("Failed to open GPIO chip\n"); 
        return 1; 
    }

    Pin12EN_out = gpiod_chip_get_line(chip, PIN_12EN);
    gpiod_line_request_output(Pin12EN_out, "12EN", GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP);
    gpiod_line_set_value(Pin12EN_out, 0);

    Pin34EN_out = gpiod_chip_get_line(chip, PIN_34EN);
    gpiod_line_request_output(Pin34EN_out, "34EN", 0);
    gpiod_line_set_value(Pin34EN_out, 0);

    return 0;
}

int init_pwm(void) {
    if (gpioInitialise() < 0) {
        printf("Failed to initialize pigpio\n");
        return 1;
    }

    //set pull up
    gpioSetPullUpDown(PIN_1A, PI_PUD_UP);
    gpioSetPullUpDown(PIN_2A, PI_PUD_UP);

    // Set GPIO18 to PWM mode
    // Frequency in Hz (eg 1000)
    // Duty cycle 0 - 255 (eg. 128 for 50%)
    gpioPWM(PIN_1A, 0);                     //set duty cycle
    gpioSetPWMfrequency(PIN_1A, PWM_FREQ);  //set frequency to 1kHz    
    gpioPWM(PIN_2A, 0);                     //set duty cycle
    gpioSetPWMfrequency(PIN_2A, PWM_FREQ);  //set frequency to 1kHz    
    gpioPWM(PIN_3A, 0);                     //set duty cycle
    gpioSetPWMfrequency(PIN_3A, PWM_FREQ);  //set frequency to 1kHz            
    gpioPWM(PIN_4A, 0);                     //set duty cycle
    gpioSetPWMfrequency(PIN_4A, PWM_FREQ);  //set frequency to 1kHz  
    
    return 0;
}

int motor_init(void)
{
    if(init_pwm() == 0) {
        if(init_gpio() == 0) {
            Motor_Stop();
            return 0;
        }
    }

    return -1;
}

void motor_deinit(void) {
    Motor_Stop();
    gpiod_chip_close(chip);
    gpioTerminate();    
}

static void Left_Forward(int duty_option) {
    int duty = 0;
    if(duty_option < 0) {
        duty = PWM_DUTY_MIN;
    }
    else if(duty_option > 0) {
        duty = PWM_DUTY_MAX;
    }
    else {
        duty = PWM_DUTY_AVG;
    }

    gpioPWM(PIN_1A, 0);
    gpioPWM(PIN_2A, duty * 255 / 100);
    gpiod_line_set_value(Pin12EN_out, 1);
}

static void Left_Backward(int duty_option) {
    int duty = 0;
    if(duty_option < 0) {
        duty = PWM_DUTY_MIN;
    }
    else if(duty_option > 0) {
        duty = PWM_DUTY_MAX;
    }
    else {
        duty = PWM_DUTY_AVG;
    }

    gpioPWM(PIN_1A, duty * 255 / 100);
    gpioPWM(PIN_2A, 0);
    gpiod_line_set_value(Pin12EN_out, 1);
}

void Motor_Stop(void)
{
    gpiod_line_set_value(Pin12EN_out, 0);
    gpiod_line_set_value(Pin34EN_out, 0);
    gpioPWM(PIN_1A, 0);
    gpioPWM(PIN_2A, 0);
    gpioPWM(PIN_3A, 0);
    gpioPWM(PIN_4A, 0);
}

static void Right_Forward(int duty_option) {
    int duty = 0;
    if(duty_option < 0) {
        duty = PWM_DUTY_MIN;
    }
    else if(duty_option > 0) {
        duty = PWM_DUTY_MAX;
    }
    else {
        duty = PWM_DUTY_AVG;
    }

    gpioPWM(PIN_3A, 0);
    gpioPWM(PIN_4A, duty * 255 / 100);
    gpiod_line_set_value(Pin34EN_out, 1);
}

static void Right_Backward(int duty_option) {
    int duty = 0;
    if(duty_option < 0) {
        duty = PWM_DUTY_MIN;
    }
    else if(duty_option > 0) {
        duty = PWM_DUTY_MAX;
    }
    else {
        duty = PWM_DUTY_AVG;
    }

    gpioPWM(PIN_3A, duty * 255 / 100);
    gpioPWM(PIN_4A, 0);
    gpiod_line_set_value(Pin34EN_out, 1);
}

void Turn_Right(void) {
    Right_Backward(-1);
    Left_Forward(-1);
}

void Turn_Left(void) {
    Right_Forward(-1);
    Left_Backward(-1);    
}

void Go_Forward(void) {
    Right_Forward(0);
    Left_Forward(0);
}

void Go_Forward_Left(void) {
    Right_Forward(1);
    Left_Forward(-1);
}

void Go_Forward_Right(void) {
    Right_Forward(-1);
    Left_Forward(1);
}

void Go_Backward(void) {
    Left_Backward(0);
    Right_Backward(0);
}

void Go_Backward_Left(void) {
    Left_Backward(-1);
    Right_Backward(1);
}

void Go_Backward_Right(void) {
    Left_Backward(1);
    Right_Backward(-1);
}
