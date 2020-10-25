#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/scb.h>

#include <stdio.h>

#include "oled.h"
#include "time.h"
#include "adc.h"
#include "menu.h"
#include "buttons.h"

int main(void) {
	rcc_clock_setup_in_hsi_out_48mhz();

	// clocks
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_ADC1);
	rcc_periph_clock_enable(RCC_I2C1);

	systick_init();

	// i2c
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6 | GPIO7);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, GPIO6 | GPIO7);
	gpio_set_af(GPIOB, GPIO_AF1, GPIO6 | GPIO7);

	rcc_set_i2c_clock_hsi(I2C1);
	i2c_reset(I2C1);
	i2c_peripheral_disable(I2C1);
	i2c_enable_analog_filter(I2C1);
	i2c_set_digital_filter(I2C1, 0);
	i2c_set_speed(I2C1, i2c_speed_sm_100k, 48);
	i2c_set_7bit_addr_mode(I2C1);
	i2c_peripheral_enable(I2C1);

	// adc
	adc_init();
	// delay_ms(5);
	// adc_calibrate(ADC1);

	disp_init();

	// inputs
	gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO12); // STUSB interrupt
	buttons_init();

	while (1) {
		buttons_update();
		menu_draw();

		// uint8_t channel_array[] = {0, 1, 2, 5};
		// adc_set_regular_sequence(ADC1, sizeof channel_array, channel_array);
		// adc_start_conversion_regular(ADC1);

		// while (!adc_eoc(ADC1));
		// uint16_t adc_iin = adc_read_regular(ADC1);
		// while (!adc_eoc(ADC1));
		// uint16_t adc_ttip = adc_read_regular(ADC1);
		// while (!adc_eoc(ADC1));
		// uint16_t adc_uin = adc_read_regular(ADC1);
		// while (!adc_eoc(ADC1));
		// uint16_t adc_tref = adc_read_regular(ADC1);

		// char buffer[20];
		// snprintf(buffer, sizeof buffer, "%d %d", adc_iin, adc_ttip);
		// disp_string_line(0, 0, buffer);
		// snprintf(buffer, sizeof buffer, "%d %d", adc_uin, adc_tref);
		// disp_string_line(0, 1, buffer);
		// disp_refresh();

		// delay_ms(150);

		// for (int x = 0; x < 96; x++) {
		// 	disp_pixel(x, 0, 1);
		// 	disp_pixel(x, 15, 1);
		// }

		// for (int y = 0; y < 16; y++) {
		// 	disp_pixel(0, y, 1);
		// 	disp_pixel(95, y, 1);
		// }

		// disp_refresh();
		// delay_ms(400);

		// disp_string_line(0, 0, "hello world");
		// disp_string_y(0, 9, "hello world");

		// disp_refresh();
		// delay_ms(400);
	}
}