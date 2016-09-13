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
#include <stdio.h>
#include "ADCSWTrigger.h"
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "Timer1.h"
#include "ST7735.h"
#include "InputOutput.h"

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
#define DUMP_SIZE 1000
#define ADC_MAX_VALUE 4096
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

uint32_t ADCdump[DUMP_SIZE];	//Buffer for ADC dump data
uint32_t TimerDump[DUMP_SIZE];//Buffer for ADC time data
uint32_t DumpIndex = 0;				//Index for buffer filling
static int32_t dataQuantities[ADC_MAX_VALUE];	//array to count number of each ADC value

volatile uint32_t ADCvalue;		//variable for ADC reads

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
  PF2 ^= 0x04;                   				// profile
  PF2 ^= 0x04;                  				// profile
  ADCvalue = ADC0_InSeq3();							// Read ADC value	
	if(DumpIndex < DUMP_SIZE){						
		ADCdump[DumpIndex] = ADCvalue;
		TimerDump[DumpIndex] = TIMER1_TAR_R;
		DumpIndex += 1;
	}
  PF2 ^= 0x04;                   				// profile
}

//This function assumes a populated global TimerDump array
//Calculates the difference between the maximum and minimum time differences
int GetTimeJitter(void){ //Uses global debug dump data to determine time jitter
	int32_t lowTime = TimerDump[0] - TimerDump[1]; 			//initialize min and max time
	int32_t highTime = lowTime;											
	int32_t hold; 
	
	for(int32_t k = 1; k < DUMP_SIZE - 1; k += 1){	//loop through buffers to find min and max times
		hold = TimerDump[k] - TimerDump[k + 1];				
		if(hold < lowTime){														
			lowTime = hold;
		}
		else if(hold > highTime){											
			highTime = hold;
		}
	}
	return highTime - lowTime;											//return time jitter					
}

//--------------GraphData--------------
//assumes an initialized static array dataQuantities
//uses ADC data from the global ADCdump, and plots based on the quantity of each measurement
//Does not support ADC ranges above 128
void GraphData(char title[]){
	for(uint32_t k = 0; k < ADC_MAX_VALUE; k += 1){	//initialize array
		dataQuantities[k] = 0;
	}
	
	for(uint32_t k = 0; k < DUMP_SIZE; k += 1){			//count quantity of each ADC value
		dataQuantities[ADCdump[k]] += 1;
	}
	//plot the data
	uint32_t minADC = ADC_MAX_VALUE;					//minimum x value for plot
	uint32_t maxADC = 0;											//maximum x value for plot
	uint32_t maxQuantity = 0;									//maximum y value for plot
	
	int32_t hold;
	for(uint32_t k = 0; k < ADC_MAX_VALUE; k += 1){ //find minX, maxX, and maxY
		hold = dataQuantities[k];
		if(hold > 0){
			maxADC = k;
			if(minADC == ADC_MAX_VALUE){
				minADC = k;
			}
			if(hold > maxQuantity){
				maxQuantity = hold;
			}
		}
	}
	ST7735_PlotClear(0, maxQuantity);	//Set range of the plot
	
	int32_t numMeasurements = maxADC - minADC + 1;
	int32_t xSize = 128;
	int32_t barWidth = 1;
	while(xSize > numMeasurements){		//get how much to scale the plot
		xSize = xSize >> 1;
		barWidth = barWidth << 1;
	}
	xSize = xSize << 1;
	barWidth = barWidth >> 1;
	
	ST7735_OutString(title);				//Display plot title
	ST7735_OutChar('\n');
	
	if(xSize > 128){								//Unsupported range
		ST7735_OutString("ADC range too high\nPlease try again");
		return;
	}
	ST7735_OutString("ADC: ");			//Display x scale
	ST7735_OutUDec(minADC);					
	ST7735_OutString(" to ");
	ST7735_OutUDec(minADC + xSize); 
	
	ST7735_OutChar('\n');													//Display y scale
	ST7735_OutString("Quantity up to ");	
	ST7735_OutUDec(maxQuantity); 

	for(int32_t k = minADC; k <= maxADC; k += 1){ //plot the PMF
		for(int i = 0; i < barWidth; i += 1){				//space the bars properly
			ST7735_PlotBar(dataQuantities[k]);
			ST7735_PlotNext();
		}
	} 
	
}

int main_PMF(void){ 										//for displaying PMF
	PLL_Init(Bus80MHz);                   // 80 MHz
	ST7735_InitR(INITR_REDTAB);						// initialize screen
  PortF_Init();           							
  ADC0_InitSWTriggerSeq3_Ch9(0);      	// activate ADC reading and interrupts
	//ADC0_InitSWTriggerSeq3_Ch9(4);   
	//ADC0_InitSWTriggerSeq3_Ch9(16);   
	//ADC0_InitSWTriggerSeq3_Ch9(64);   
  Timer0A_Init100HzInt();               // set up Timer0A for 100 Hz interrupts
  PF2 = 0;                      				// Turn off LED
	Timer1_Init_FullTime();								// Initialize Timer 1 to run for 53 seconds
  EnableInterrupts();
  while(DumpIndex < DUMP_SIZE){
    PF1 ^= 0x02;												// Toggles LED and causes time jitter
		volatile int32_t div = 787778873;		// create jitter
		div /= 77777;
  }
	GraphData("ADC Plot");
	while(1){
		PF1 ^= 0x02;
	}
	return 0;
}

int main(void){ //for jitter
	PLL_Init(Bus80MHz);                   // 80 MHz
	ST7735_InitR(INITR_REDTAB);						// initialize screen
  PortF_Init();           							// Initialize port F
  ADC0_InitSWTriggerSeq3_Ch9(0);     		// activate ADC reading and interrupts
  Timer0A_Init100HzInt();               // set up Timer0A for 100 Hz interrupts
  PF2 = 0;                      				// Turn off LED
	Timer1_Init_FullTime();								// Initialize Timer 1 to run for 53 seconds
  EnableInterrupts();
  while(DumpIndex < DUMP_SIZE){
    PF1 ^= 0x02;												// Toggles LED and causes time jitter
		volatile int32_t div = 787778873;		// create jitter
		div /= 77777;
  }
	ST7735_OutUDec(GetTimeJitter()); 			//print time jitter to ST7735
	ST7735_OutString(" cycles\n"); 				
	while(1){
		PF1 ^= 0x02;
	}																			
	return 0;
}

int main_line(void){ //draw lines
	PLL_Init(Bus80MHz);                   // 80 MHz
	ST7735_InitR(INITR_REDTAB);						// initialize screen
																				//display test lines
	ST7735_Line(60, 0, 0, 0, ST7735_YELLOW); 		//horizontal	
	ST7735_Line(60, 0, 10, 10, ST7735_RED);			//shallow
	ST7735_Line(60, 0, 20, 40, ST7735_GREEN);		//45 degrees
	ST7735_Line(60, 0, 50, 60, ST7735_BLUE);		//steep
	ST7735_Line(60, 0, 60, 70, ST7735_WHITE);		//vertical
	ST7735_Line(60, 0, 70, 60, ST7735_CYAN);		//steep
	ST7735_Line(60, 0, 100, 40, ST7735_MAGENTA);//45 degrees
	ST7735_Line(60, 0, 110, 10, ST7735_YELLOW);	//shallow	
	
	ST7735_Line(60, 159, 10, 145, ST7735_YELLOW);//shallow	
	ST7735_Line(60, 159, 20, 119, ST7735_RED);		//45
	ST7735_Line(60, 159, 40, 99, ST7735_GREEN);	//steep
	ST7735_Line(60, 159, 60, 90, ST7735_BLUE);	//vertical
	ST7735_Line(60, 159, 80, 99, ST7735_WHITE);	//steep xxxx
	ST7735_Line(60, 159, 100, 119, ST7735_CYAN);	//45
	ST7735_Line(60, 159, 110, 145, ST7735_MAGENTA);//shallow
	ST7735_Line(60, 159, 127, 159, ST7735_YELLOW);//horizontal
	
	return 0;
}

