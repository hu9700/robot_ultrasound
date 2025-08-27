#include <gpiod.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <stack>
#include "sensor.h"

extern gpiod_chip *chip;
extern volatile bool is_stop;
gpiod_line *in = NULL;
gpiod_line *out = NULL;
static std::mutex sensor_data_stack_mtx;
static std::condition_variable sensor_data_stack_cv;
static std::stack<sensor_data> sensor_data_stack;

#define ECHO_LINE       (27)
#define TRIGGER_LINE    (17)

int sensor_init(void) {
    if(chip) {
        in = gpiod_chip_get_line(chip, ECHO_LINE);
        out = gpiod_chip_get_line(chip, TRIGGER_LINE);
        if(in && out) {
            gpiod_line_request_both_edges_events(in, "pulse-measure");
            gpiod_line_request_output(out, "pulse-gen", 0);   

            return 0;
        }
        else
        {
            std::cerr << "gpiod_chip_get_line fail" << std::endl;
        }
    }
    else {
        std::cerr << "chip create fail" << std::endl;
    }
    return -1;
}

void sensor_datapush(const sensor_data &data) {
    std::lock_guard<std::mutex> lock(sensor_data_stack_mtx);
    sensor_data_stack.push(data);
    sensor_data_stack_cv.notify_one();
}

sensor_data sensor_getdata(long timeout_ms) {
    {
        std::lock_guard<std::mutex> lock(sensor_data_stack_mtx);
        if(!sensor_data_stack.empty()) {
            sensor_data data = sensor_data_stack.top();
            //clear the stack
            do{
                sensor_data_stack.pop();
            } while(!sensor_data_stack.empty());
            //check data emmit time
            auto since_ms = std::chrono::duration_cast<std::chrono::milliseconds>(data.timestamp.time_since_epoch());
            if(since_ms.count() < timeout_ms / 2) {//the lastest data is less than timeout_ms/2 before
                std::cout << data.distance << std::endl;
                return data;
            }
        }
    }

    {
        //stack is empty or last data is out of date
        std::unique_lock<std::mutex> lock(sensor_data_stack_mtx);
        if (sensor_data_stack_cv.wait_for(lock, std::chrono::milliseconds(timeout_ms)
                                        , []{ return !sensor_data_stack.empty(); })) {
            //std::lock_guard<std::mutex> lock(sensor_data_stack_mtx);
            sensor_data data = sensor_data_stack.top();
            //clear the stack
            do{
                sensor_data_stack.pop();
            } while(!sensor_data_stack.empty());
            
            std::cout << data.distance << std::endl;
            return data;
        }
        else {//timeout
            sensor_data data = {-1, std::chrono::system_clock::now()};
            return data;
        }
    }
}

void sensor_loop(void) {
    while (!is_stop) {

        // Generate a 10µs pulse, use 1ms to be better generation
        gpiod_line_set_value(out, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        gpiod_line_set_value(out, 0);

        struct gpiod_line_event event_r, event_f;
        // Wait for rising edge
        timespec timeout_r;
        timespec timeout_f;
        timeout_r.tv_nsec = 10000000;
        timeout_f.tv_nsec = 190000000;
        if (gpiod_line_event_wait(in, &timeout_r) > 0) {
            gpiod_line_event_read(in, &event_r);
        }
        else {
            continue;
        }

        // Wait for falling edge
        if (gpiod_line_event_wait(in, &timeout_f) > 0) {
            gpiod_line_event_read(in, &event_f);
        }
        else {
            continue;
        }

        auto t1 = std::chrono::nanoseconds(event_r.ts.tv_nsec) +
                  std::chrono::seconds(event_r.ts.tv_sec);
        auto t2 = std::chrono::nanoseconds(event_f.ts.tv_nsec) +
                  std::chrono::seconds(event_f.ts.tv_sec);

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        float distance = (duration / 2) * 349 / 10000;

        if(distance < 400 && distance > 4) {
            //std::cout << "Distance " << distance << " cm\n";
            sensor_data data = {distance, std::chrono::system_clock::now()};
            sensor_datapush(data);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}