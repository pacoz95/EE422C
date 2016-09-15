/****************Globals.h******************
*This file contains definitions and module usage information for all global variables in Lab3
*Paul Cozzi and Eric Li, 9/14/2016
********************************************/

/************CurrentTime*************
*used as communication of the time between Timer0 and Clock
*Keeps track of the current time
*CurrentTime[0-7] are seconds, 
*CurrentTime[8-15] are minutes, 
*CurrentTime[16-23] are hours,
*CurrentTime[24-31] are junk
*Initialized to 12:00:00
*************************************/
int32_t CurrentTime = 0x000C0000;

/************Switch Trigger indicators********
*These will be set in their respective switch interrupts
*cleared when they are serviced (in whatever function they're used in)
**********************************************/
int32_t SW1Triggered = 0; //PE0
int32_t SW2Triggered = 0; //PE1
int32_t SW3Triggered = 0; //PE2
int32_t SW4Triggered = 0; //PE3

/************stateAction**************
*main will call a different function based on the state of the system
*this is the variable that holds that function
**************************************/

void (*stateAction)(void);						//perform a different function depending on the state of the system
																						//set in interrupts that change system state

