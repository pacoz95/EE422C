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
