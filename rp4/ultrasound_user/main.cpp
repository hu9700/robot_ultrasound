#include <gpiod.h>
#include <iostream>
#include <chrono>
#include <thread>

#define GPIO_CHIP "/dev/gpiochip0"
#define ECHO_LINE       (27)
#define TRIGGER_LINE    (17)

int main() {
    gpiod_chip *chip = gpiod_chip_open(GPIO_CHIP);
    if (!chip) { 
        std::cerr << "Failed to open GPIO chip\n"; 
        return 1; 
    }

    gpiod_line *in = gpiod_chip_get_line(chip, ECHO_LINE);
    gpiod_line *out = gpiod_chip_get_line(chip, TRIGGER_LINE);
    gpiod_line_request_both_edges_events(in, "pulse-measure");
    gpiod_line_request_output(out, "pulse-gen", 0);

    while (true) {
        

        // Generate a 10µs pulse
        gpiod_line_set_value(out, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        gpiod_line_set_value(out, 0);
#if 1
        struct gpiod_line_event event_r, event_f;
        // Wait for rising edge
        if (gpiod_line_event_wait(in, nullptr) > 0) {
            gpiod_line_event_read(in, &event_r);
        }

        // Wait for falling edge
        if (gpiod_line_event_wait(in, nullptr) > 0) {
            gpiod_line_event_read(in, &event_f);
        }

        auto t1 = std::chrono::nanoseconds(event_r.ts.tv_nsec) +
                  std::chrono::seconds(event_r.ts.tv_sec);
        auto t2 = std::chrono::nanoseconds(event_f.ts.tv_nsec) +
                  std::chrono::seconds(event_f.ts.tv_sec);

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        long distance = (duration / 2) * 349 / 10000;
        //std::cout << "Pulse width: " << duration << " us\n";

        if(distance < 400 && distance > 4) {
            std::cout << "Distance " << distance << " cm\n";
        }
#endif
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    gpiod_chip_close(chip);
    return 0;
}