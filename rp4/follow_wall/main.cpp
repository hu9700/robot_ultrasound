#include <thread>
#include <iostream>
#include <gpiod.h>
#include <thread>
#include <signal.h>
#include <pigpio.h>

#include "sensor.h"
#include "motor.h"

#define GPIO_CHIP "/dev/gpiochip0"
#define CONTROL_LOOP_TIME       (100)//ms
#define SENSOR_TIMEOUT          (50)//ms
#define REF_DISTANCE            (15)//(10)//cm

gpiod_chip *chip = NULL;
volatile bool is_stop = false;

void handle_signal(int sig) {
    std::cout << "stop" << std::endl;
    is_stop = true;
}

int init(void) {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    chip = gpiod_chip_open(GPIO_CHIP);

    if (!chip) { 
        std::cerr << "Failed to open GPIO chip\n"; 
        return 1; 
    }

    if(sensor_init() != 0) {
        std::cerr << "Failed to init sensor \n"; 
        return 1;
    }

    if(init_motor() != 0) {
        std::cerr << "Failed to init motor \n"; 
        return 1;        
    }

    return 0;
}

void deinit(void) {
    if(chip) {
        gpiod_chip_close(chip);
    }
    gpioTerminate();
}

#define MOTOR_P     (0.2)
#define MOTOR_D     (0.00)

void control_algo(float distance) {
    static float error_pre = 0;
    float error = distance - REF_DISTANCE;
    //error = 0 - error;
    float control = error * MOTOR_P + (MOTOR_D * (error - error_pre));
    error_pre = error;
#if 1    
    Go_Forward_Turn(control);
#else    
    Go_Turn(control);
#endif
}

#define TIMEOUT_COUNT_MAX   (15)
void control_loop(void) {
    static int count_timeout = 0;
    while (!is_stop) {
        auto start_time = std::chrono::system_clock::now();

        sensor_data data = sensor_getdata(SENSOR_TIMEOUT);
        if(data.distance > 0) {
            count_timeout = 0;
            control_algo(data.distance);
        }
        else {//error or timeout happened sensor_getdata
            count_timeout ++;
            if(count_timeout < TIMEOUT_COUNT_MAX) {
                motor_stop();
            }
            else {
                count_timeout = 0;
                Go_Forward_Turn(-1);
            }
            std::cout << "data time out" << std::endl;
        }

        std::this_thread::sleep_until(start_time + std::chrono::milliseconds(CONTROL_LOOP_TIME));
    }
}

int main(int argc, char **argv) {
    if(init() == 0) {
        std::thread sensor_thread(sensor_loop);
        std::thread control_thread(control_loop);

        control_thread.join();
        sensor_thread.join();    

        motor_stop();
    }

    deinit();
    return 0;
}