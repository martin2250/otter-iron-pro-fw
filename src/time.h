#ifndef __TIME_H__
#define __TIME_H__

#include <stdint.h>

extern volatile uint32_t sys_ms;
void delay_ms(uint32_t time);
void systick_init(void);

#endif // __TIME_H__
