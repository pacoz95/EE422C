//file name SwitchInit
//should initialize the switches

#include <stdio.h>
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "SwitchInit.h"

void Switch_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x10; //activate clock for Port E
	while ((SYSCTL_PRGPIO_R&0x10) != 0x10){};  //ready
	GPIO_PORTE_AFSEL_R &= ~0x0F;  //GPIO function on PE3-0
	GPIO_PORTE_AMSEL_R &= ~0x0F;  //disable analog function on PE3-0
	GPIO_PORTE_PCTL_R &= ~0x0000FFFF;  //config PE3-0 as GPIO
	GPIO_PORTE_DIR_R &= ~0x0F;  //set direction register
	GPIO_PORTE_DEN_R = 0x0F;  //enable digital port
	GPIO_PORTE_PUR_R = 0x0F;  //set pull up enable bits
	GPIO_PORTE_RIS_R = 0x0F;  //ask about ris ic and stuff
}
