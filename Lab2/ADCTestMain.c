// ADCTestMain.c
// Runs on TM4C123
// This program periodically samples ADC channel 0 and stores the
// result to a global variable that can be accessed with the JTAG
// debugger and viewed with the variable watch feature.
// Daniel Valvano - Edited by Eric Li and Paul Cozzi
// September 5, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// center of X-ohm potentiometer connected to PE3/AIN0
// bottom of X-ohm potentiometer connected to ground
// top of X-ohm potentiometer connected to +3.3V 
#include <stdint.h>
#include <stdio.h> //why do we have to include this to include ST7735.h?
#include "ADCSWTrigger.h"
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "Timer1.h"
#include "fixed.h"
#include "ST7735.h"

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
#define DUMP_SIZE 1000
#define ADC_MAX_VALUE 4096
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

volatile uint32_t ADCdump[DUMP_SIZE];
volatile uint32_t TimerDump[DUMP_SIZE];
volatile uint32_t DumpIndex = 0;

volatile uint32_t ADCvalue;
// This debug function initializes Timer0A to request interrupts
// at a 100 Hz frequency.  It is similar to FreqMeasure.c.
void Timer0A_Init100HzInt(void){
  volatile uint32_t delay;
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER0_TAILR_R = 799999;         // start value for 100 Hz interrupts
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC
}
void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
  PF2 ^= 0x04;                   // profile
  PF2 ^= 0x04;                   // profile
  ADCvalue = ADC0_InSeq3();
	if(DumpIndex < DUMP_SIZE){
		ADCdump[DumpIndex] = ADCvalue;
		TimerDump[DumpIndex] = TIMER1_TAR_R;
		DumpIndex += 1;
	}
  PF2 ^= 0x04;                   // profile
}

int GetTimeJitter(void){ //Uses global debug dump data to determine time jitter
	int lowTime = TimerDump[0] - TimerDump[1];
	int highTime = lowTime;
	int hold; 
	
	for(int32_t k = 1; k < DUMP_SIZE - 1; k += 1){
		hold = TimerDump[k] - TimerDump[k + 1];
		if(hold < lowTime){
			lowTime = hold;
		}
		else if(hold > highTime){
			highTime = hold;
		}
	}
	return highTime - lowTime;
}


void GraphData(){
	int dataQuantities[ADC_MAX_VALUE];
	for(uint32_t k = 0; k < ADC_MAX_VALUE; k += 1){
		dataQuantities[k] = 0;
	}
	
	for(uint32_t k = 0; k < 1000; k += 1){
		dataQuantities[ADCdump[k]] += 1;
	}
	//TODO: Print the data...?
}

int main_old(void){
  PLL_Init(Bus80MHz);                   // 80 MHz
  SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
  ADC0_InitSWTriggerSeq3_Ch9();         // allow time to finish activating
  Timer0A_Init100HzInt();               // set up Timer0A for 100 Hz interrupts
  GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
  PF2 = 0;                      // turn off LED
  EnableInterrupts();
  while(1){
    PF1 ^= 0x02;  // toggles when running in main
  }
}

int main(void){ //for jitter
	PLL_Init(Bus80MHz);                   // 80 MHz
	ST7735_InitR(INITR_REDTAB);						// initialize screen
  SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
  ADC0_InitSWTriggerSeq3_Ch9();         // allow time to finish activating
  Timer0A_Init100HzInt();               // set up Timer0A for 100 Hz interrupts
  GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
  PF2 = 0;                      				// Turn off LED
	Timer1_Init_FullTime();								// Initialize Timer 1 to run for 53 seconds
  EnableInterrupts();
  while(DumpIndex < DUMP_SIZE){
    PF1 ^= 0x02;  // toggles when running in main
		DisableInterrupts();
		int m = 1000000;
		m /= 77777;
		m /= 6;
		m /= 26;
		EnableInterrupts();
  }
	ST7735_OutUDec(GetTimeJitter()); 
	ST7735_OutString(" cycles\n");
	return 0;
}

