#ifndef __OLED_H__
#define __OLED_H__

#include <stdint.h>

#define OLED_WIDTH 96
#define OLED_HEIGHT 16

void disp_init(void);
void disp_on(void);
void disp_off(void);
void disp_clear(void);
void disp_refresh(void);
void disp_pixel(uint8_t x, uint8_t y, uint8_t color);
void disp_char_y(char c, uint8_t x, uint8_t y);
void disp_char_line(char c, uint8_t x, uint8_t line);
void disp_string_y(uint8_t x, uint8_t y, const char* str);
void disp_string_line(uint8_t x, uint8_t line, const char* str);

#endif // __OLED_H__
