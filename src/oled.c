#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <string.h>
#include "oled.h"
#include "time.h"
#include "font.h"

#define OLED_SETCONTRAST 0x81
#define OLED_DISPLAYALLON_RESUME 0xA4
#define OLED_DISPLAYALLON 0xA5
#define OLED_NORMALDISPLAY 0xA6
#define OLED_INVERTDISPLAY 0xA7
#define OLED_DISPLAYOFF 0xAE
#define OLED_DISPLAYON 0xAF
#define OLED_SETDISPLAYOFFSET 0xD3
#define OLED_SETCOMPINS 0xDA
#define OLED_SETVCOMDETECT 0xDB
#define OLED_SETDISPLAYCLOCKDIV 0xD5
#define OLED_SETPRECHARGE 0xD9
#define OLED_SETMULTIPLEX 0xA8
#define OLED_SETLOWCOLUMN 0x00
#define OLED_SETHIGHCOLUMN 0x10
#define OLED_SETSTARTLINE 0x40
#define OLED_MEMORYMODE 0x20
#define OLED_COMSCANINC 0xC0
#define OLED_COMSCANDEC 0xC8
#define OLED_SEGREMAP 0xA0
#define OLED_CHARGEPUMP 0x8D

#define OLED_ADDR 0x3c

#define OLED_BUFSIZE (OLED_WIDTH * OLED_HEIGHT / 8)

struct {
	uint8_t refresh_cmd[17];
	uint8_t buffer[OLED_BUFSIZE];
} disp_buffer = {
	{0x80, 0xAF, 0x80, 0x21, 0x80, 0x20, 0x80, 0x7F, 0x80, 0xC0, 0x80, 0x22, 0x80, 0x00, 0x80, 0x01, 0x40},
	{0}
};

void disp_on(void) {
	uint8_t cmd = OLED_DISPLAYON;
	i2c_transfer7(I2C1, OLED_ADDR, &cmd, 1, NULL, 0);
}

void disp_off(void) {
	uint8_t cmd = OLED_DISPLAYOFF;
	i2c_transfer7(I2C1, OLED_ADDR, &cmd, 1, NULL, 0);
}

void disp_init(void) {
	// lcd reset
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO10);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO10);

	gpio_clear(GPIOA, GPIO10);
	delay_ms(2);
	gpio_set(GPIOA, GPIO10);
	delay_ms(50);

	disp_off();
	disp_refresh();

	uint8_t s[] = {
		0x80, 0xD5, /*Set display clock divide ratio / osc freq*/
		0x80, 0x80, /*Divide ratios*/
		0x80, 0xA8, /*Set Multiplex Ratio*/
		0x80, 0x0F, /*16 == max brightness,39==dimmest*/
		0x80, 0xC8, /*Set COM Scan direction*/
		0x80, 0xD3, /*Set vertical Display offset*/
		0x80, 0x00, /*0 Offset*/
		0x80, 0x40, /*Set Display start line to 0*/
		0x80, 0xA0, /*Set Segment remap to normal*/
		0x80, 0x8D, /*Charge Pump*/
		0x80, 0x14, /*Charge Pump settings*/
		0x80, 0xDA, /*Set VCOM Pins hardware config*/
		0x80, 0x02, /*Combination 2*/
		0x80, 0x81, /*Contrast*/
		0x80, 0x33, /*^51*/
		0x80, 0xD9, /*Set pre-charge period*/
		0x80, 0x22, /*Pre charge period*/
		0x80, 0xDB, /*Adjust VCOMH regulator ouput*/
		0x80, 0x40, /*VCOM level*/
		0x80, 0xA4, /*Enable the display GDDR*/
		0x80, 0XA6, /*Normal display*/
		0x80, 0x20, /*Memory Mode*/
		0x80, 0x00 /*Wrap memory*/
	};
  
	i2c_transfer7(I2C1, OLED_ADDR, s, sizeof s, NULL, 0);
	disp_refresh();
	delay_ms(100);
	disp_on();
}

void disp_clear(void) {
	memset(disp_buffer.buffer, 0, sizeof disp_buffer.buffer);
}

void disp_refresh(void) {
	i2c_transfer7(I2C1, OLED_ADDR, (uint8_t*)&disp_buffer, sizeof disp_buffer, NULL, 0);
}

void disp_pixel(uint8_t x, uint8_t y, uint8_t color) {
	if (x > 95 || y > 15) {
		return;
	}
	uint8_t *addr = disp_buffer.buffer + x + OLED_WIDTH * (y/8);
	if(color == 1){
		*addr |=  (1 << y % 8);
	} else if (color == 0){
		*addr &= ~(1 << y % 8);
	}
}

void disp_char_y(char c, uint8_t x, uint8_t y) {
	uint8_t cc = c & 0x7F;
    if (cc < ' ') {
        cc = 0;
    } else {
        cc -= ' ';
    }
    for (uint8_t j=0; j<FONT_WIDTH; j++) {
        for (uint8_t i=0; i<FONT_HEIGHT; i++) {
			disp_pixel(x+j, y+i, (font[cc][j] >> i) & 0x01);
        }
    }
}

void disp_char_line(char c, uint8_t x, uint8_t line) {
	if (x >= (OLED_WIDTH - FONT_WIDTH) || line >= (OLED_HEIGHT / FONT_HEIGHT)) {
		return;
	}
	uint8_t cc = c & 0x7F;
    if (cc < ' ') {
        cc = 0;
    } else {
        cc -= ' ';
    }
    for (uint8_t j=0; j<FONT_WIDTH; j++) {
        disp_buffer.buffer[line * OLED_WIDTH + x + j] = font[cc][j];
    }
}

void disp_string_y(uint8_t x, uint8_t y, const char* str) {
    while (*str) {
        disp_char_y(*str++, x, y);
        x += FONT_WIDTH;
    }
}

void disp_string_line(uint8_t x, uint8_t line, const char* str) {
    while (*str) {
        disp_char_line(*str++, x, line);
        x += FONT_WIDTH;
    }
}