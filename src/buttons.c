#include <libopencm3/stm32/gpio.h>

#include "buttons.h"
#include "menu.h"
#include "time.h"

// todo: move settings to one file
#define SETTING_BUTTON_DELAY_MS 30

struct {
    uint32_t time_fire;
    uint8_t state_last;
    bool wait_release;
} buttons;

void buttons_update(void) {
    bool up = !!(gpio_port_read(GPIOA) & GPIO3);
    bool down = !!(gpio_port_read(GPIOC) & GPIO13);
    uint8_t state = up << 1 | down;

    uint32_t now = sys_ms;
    if (state == 0) {
        buttons.wait_release = false;
        buttons.state_last = 0;
        return;
    }
    if (buttons.wait_release) {
        return;
    }
    if (state != buttons.state_last) {
        buttons.state_last = state;
        buttons.time_fire = now + SETTING_BUTTON_DELAY_MS;
        return;
    }
    if (now > buttons.time_fire) {
        menu_button(!!(state & 2), !!(state & 1));
        buttons.wait_release = true;
    }
}

void buttons_init(void) {
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, GPIO3); // up
	gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, GPIO13); // down / DFU
}