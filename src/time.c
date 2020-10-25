#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>

#include "time.h"

volatile uint32_t sys_ms = 0;

void sys_tick_handler(void) {
	sys_ms++;
}

void delay_ms(uint32_t time) {
	uint32_t end = sys_ms + time;
	while (sys_ms < end);
}

void systick_init(void) {
    systick_set_frequency(1000, 48000000);
	systick_interrupt_enable();
	systick_counter_enable();
}