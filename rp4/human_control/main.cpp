#include "keyhandle.h"
#include "motor.h"

int main(int argc, char ** argv) {
    if(motor_init() == 0) {
        key_handle();
        motor_deinit();
    }
    return 0;
}