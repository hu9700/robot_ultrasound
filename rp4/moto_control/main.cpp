#include <pigpio.h>
#include <stdio.h>
#include <gpiod.h>
#include <signal.h>

#include <chrono>
#include <thread>

#include <cmath>
/*
26  1/2 EN
16  3/4 EN
19  1A
18  2A
13  3A
12  4A
*/

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
volatile unsigned char is_stop = 0;

void handle_signal(int sig) {
    is_stop = 1;
}

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

void Left_Forward(float duty) {
  gpiod_line_set_value(Pin12EN_out, 0);
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  gpioPWM(PIN_1A, 0);
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  gpioPWM(PIN_2A, PWM_DUTY_MIN * 255 / 100);
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  gpiod_line_set_value(Pin12EN_out, 1);
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void Left_Backward(float duty) {
  gpiod_line_set_value(Pin12EN_out, 0);
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  gpioPWM(PIN_1A, PWM_DUTY_MIN * 255 / 100);
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  gpioPWM(PIN_2A, 0);
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  gpiod_line_set_value(Pin12EN_out, 1);
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void Left_Stop(void) {
    gpiod_line_set_value(Pin12EN_out, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    gpioPWM(PIN_1A, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    gpioPWM(PIN_2A, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void Right_Stop(void) {
    gpiod_line_set_value(Pin34EN_out, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    gpioPWM(PIN_3A, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    gpioPWM(PIN_4A, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void Right_Forward(float duty) {
  gpiod_line_set_value(Pin34EN_out, 0);
  gpioPWM(PIN_3A, 0);
  gpioPWM(PIN_4A, PWM_DUTY_MIN * 255 / 100);
  gpiod_line_set_value(Pin34EN_out, 1);
}

void Right_Backward(float duty) {
  gpiod_line_set_value(Pin34EN_out, 0);
  gpioPWM(PIN_3A, PWM_DUTY_MIN * 255 / 100);
  gpioPWM(PIN_4A, 0);
  gpiod_line_set_value(Pin34EN_out, 1);
}

void Turn_Right(void) {
    Right_Forward(0);
    Left_Backward(0);
}

void Turn_Left(void) {
    Right_Backward(0);
    Left_Forward(0);
}

//turn bias negative, turn left
//turn bias positive, turn right
//turn bias is between -1 to 1
void Go_Forward_Turn(float turn_bias) {
    int left_pwm = PWM_DUTY_AVG + 7;
    int right_pwm = PWM_DUTY_AVG;

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
        left_pwm = PWM_DUTY_MIN + std::abs((PWM_DUTY_MAX - PWM_DUTY_MIN) * turn_bias);
    }
    else {
        right_pwm = PWM_DUTY_MIN + std::abs((PWM_DUTY_MAX - PWM_DUTY_MIN) * turn_bias);
    }

    gpioPWM(PIN_1A, 0);
    gpioPWM(PIN_2A, left_pwm * 255 / 100);
    gpioPWM(PIN_3A, 0);
    gpioPWM(PIN_4A, right_pwm * 255 / 100);    
    gpiod_line_set_value(Pin12EN_out, 1);
    gpiod_line_set_value(Pin34EN_out, 1);    
}

int init(void) {
    if(init_pwm() == 0) {
        if(init_gpio() == 0) {
            signal(SIGINT, handle_signal);
            signal(SIGTERM, handle_signal);
            return 0;
        }
    }

    return -1;
}

void test_direction(void) {
    printf("Left Forward\n");
    Left_Forward(0);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    Left_Stop();

    printf("Right Forward\n");
    Right_Forward(0);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    Right_Stop();
    
    printf("Left Backward\n");
    Left_Backward(0);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    Left_Stop();
    
    printf("Right Backward\n");
    Right_Backward(0);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    Right_Stop();
}

void test_turn(void) {
#if 1    
    for(int i = 0; i < 10; i ++) {
        Go_Forward_Turn(-0.5 + (i % 2));
        //Go_Turn(-0.5 + (i % 2));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
#else
    Go_Forward_Turn(0);
    std::this_thread::sleep_for(std::chrono::seconds(3));
#endif
}

int main(int argc, char **argv) {
    if(init() == 0) {
        while(!is_stop) {
            //test_direction();
            test_turn();
        }
    }

    Left_Stop();
    Right_Stop();
    gpiod_chip_close(chip);
    gpioTerminate();

    return 0;
}