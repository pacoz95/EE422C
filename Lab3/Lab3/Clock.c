/********************Clock.c******************
*contains code for controlling a clock (main menu)
*display on the ST7735 LCD
*Paul Cozzi and Eric Li, 9/15/2016
**********************************************/
#include <stdint.h>
#include <stdio.h>
#include "Alarm.h"
#include "Clock.h"
#include "ST7735.h"

void Clock_Init(void){
	ST7735_InitR(INITR_REDTAB);								//initialize the screen
	Alarm_Init();															//initialize the alarm system
}

void Clock_MainPageDigital(void){
}

void Clock_MainPageAnalog(void){
}
