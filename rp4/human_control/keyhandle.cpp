#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <set>
#include <map>
#include <signal.h>
#include <thread>
#include <chrono>
#include "keyhandle.h"
#include "motor.h"

using motor_func = void (*)(void);
static std::map<char, motor_func> key_func_map = {
    {'a', Turn_Left},
    {'d', Turn_Right},
    {'w', Go_Forward},
    {'s', Go_Backward},
    {'q', Go_Forward_Left},
    {'e', Go_Forward_Right},
    {'z', Go_Backward_Left},
    {'c', Go_Backward_Right}
};

struct termios oldSettings, newSettings;

static void setupTerminal() {
    // Get current terminal settings
    tcgetattr(STDIN_FILENO, &oldSettings);
    // Make new settings same as old ones
    newSettings = oldSettings;
    // Disable canonical mode and echo
    newSettings.c_lflag &= ~(ICANON | ECHO);
    // Apply new settings
    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
    // Make stdin non-blocking
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

static void restoreTerminal() {
    // Restore old terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
}

bool is_stop = false;

static void handle_signal(int sig) {
    std::cout << "stop" << std::endl;
    is_stop = true;
}

static void key_to_func(char key) {
    static char key_pre = 0;

    if(key != key_pre) {
        if(key_func_map.count(key)){
            (key_func_map[key])();
        }
    }
}

void key_handle() {
    static char c_pre = 0;
    std::set<char> pressedKeys;
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    setupTerminal();

    while(!is_stop) {
        char c = 0;
        pressedKeys.clear();
        if(read(STDIN_FILENO, &c, 1) > 0) {
            //Handle key press
            if (c == 'a' || c == 's' || c == 'd' || c == 'w' || c == 'q' || c == 'e' || c == 'z' || c == 'c') {
                pressedKeys.insert(c);
            }
        }
        else {//no key press
            if(c_pre != 0) {
                Motor_Stop();
            }
        }
        if(pressedKeys.size() == 1) {
            key_to_func(c);
            std::cout << c << std::endl;
        }

        c_pre = c;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    restoreTerminal();
}