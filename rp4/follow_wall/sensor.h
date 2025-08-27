#ifndef __SENSOR_H__
#define __SENSOR_H__
#include <chrono>

struct sensor_data {
    float distance;
    std::chrono::_V2::system_clock::time_point timestamp;
};

int sensor_init(void);
void sensor_loop(void);
sensor_data sensor_getdata(long timeout_ms);

#endif