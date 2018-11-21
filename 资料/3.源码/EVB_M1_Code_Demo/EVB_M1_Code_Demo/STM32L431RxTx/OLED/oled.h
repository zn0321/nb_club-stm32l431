#ifndef __OLED__
#define __OLED__
#include "stm32L4xx_hal.h"
#include "u8g_arm.h"



void OLED_Show_Page1(void);
void OLED_Show_Page2(void);
void OLED_Show_a_Page(int show_i);

#endif


