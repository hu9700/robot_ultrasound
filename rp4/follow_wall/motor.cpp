#include <pigpio.h>
#include <gpiod.h>
#include <chrono>
#include <thread>
#include <cmath>
#include <stdio.h>

#define PWM_FREQ    (1000)//HZ
#define PWM_DUTY_MIN    (35)
#define PWM_DUTY_AVG    (45)
#define PWM_DUTY_MAX    (55)

#define PIN_12EN    (26)
#define PIN_34EN    (16)
#define PIN_1A      (19)
#define PIN_2A      (18)
#define PIN_3A      (13)
#define PIN_4A      (12)

extern gpiod_chip *chip;
gpiod_line *Pin12EN_out = NULL;
gpiod_line *Pin34EN_out = NULL;

int init_gpio(void) {
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

int init_motor(void) {
    if(init_pwm() == 0) {
        if(init_gpio() == 0) {
            return 0;
        }
    }

    return -1;
}

void motor_stop(void) {
    gpiod_line_set_value(Pin12EN_out, 0);
    gpiod_line_set_value(Pin34EN_out, 0);

    gpioPWM(PIN_1A, 0);
    gpioPWM(PIN_2A, 0);
    gpioPWM(PIN_3A, 0);
    gpioPWM(PIN_4A, 0);
}

//turn bias negative, turn left
//turn bias positive, turn right
//turn bias is between -1 to 1
void Go_Forward_Turn(float turn_bias) {
    int left_pwm = PWM_DUTY_AVG;
    int right_pwm = PWM_DUTY_AVG + 10;

    printf("%f\n", turn_bias);

    if(turn_bias < -1) {
        turn_bias = -1;
    }
    if(turn_bias > 1) {
        turn_bias = 1;
    }

    if(turn_bias < 0) {
        right_pwm += std::abs((PWM_DUTY_MAX - PWM_DUTY_AVG) * turn_bias);
        left_pwm -= std::abs((PWM_DUTY_AVG - PWM_DUTY_MIN) * turn_bias);
    }
    else {
        left_pwm += std::abs((PWM_DUTY_MAX - PWM_DUTY_AVG) * turn_bias);
        right_pwm -= std::abs((PWM_DUTY_AVG - PWM_DUTY_MIN) * turn_bias);
    }

    gpioPWM(PIN_1A, 0);
    gpioPWM(PIN_2A, left_pwm * 255 / 100);
    gpioPWM(PIN_3A, 0);
    gpioPWM(PIN_4A, right_pwm * 255 / 100);    
    gpiod_line_set_value(Pin12EN_out, 1);
    gpiod_line_set_value(Pin34EN_out, 1);
}

void Go_Turn(float turn_bias) {
    int left_pwm = 0;
    int right_pwm = 0;

    printf("%f\n", turn_bias);

    if(turn_bias < -1) {
        turn_bias = -1;
    }
    if(turn_bias > 1) {
        turn_bias = 1;
    }

    if(turn_bias < 0) {
        right_pwm = PWM_DUTY_MIN + std::abs((PWM_DUTY_MAX - PWM_DUTY_MIN) * turn_bias);
        
    }
    else {
        left_pwm = PWM_DUTY_MIN + std::abs((PWM_DUTY_MAX - PWM_DUTY_MIN) * turn_bias);
    }

    gpioPWM(PIN_1A, 0);
    gpioPWM(PIN_2A, left_pwm * 255 / 100);
    gpioPWM(PIN_3A, 0);
    gpioPWM(PIN_4A, right_pwm * 255 / 100);    
    gpiod_line_set_value(Pin12EN_out, 1);
    gpiod_line_set_value(Pin34EN_out, 1);    
}