#ifndef __DELAY_H
#define __DELAY_H 			   
#include "sys.h"

void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u32 nus);

/* Optional RTOS hooks. Return non-zero when handled by RTOS layer. */
int delay_rtos_ms(u32 nms);
int delay_rtos_us(u32 nus);

#endif





























