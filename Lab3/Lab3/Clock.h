/*****************Clock.h*******************
*These functions are used to interface with the clock
*Clock display depends only on a 32-bit global integer that holds the time, "Current  Time"
*Paul Cozzi, Eric Li, 9/14/2016
********************************************/

/*****************Clock_Init****************
*Initialize the clock module
********************************************/
void Clock_Init(void);


/*****************Clock_MainPageDigital******
*Function to display time in digital format
*and change to the appropriate other
*main function if necessary
*********************************************/
void Clock_MainPageDigital(void);

/*****************Clock_MainPageAnalog******
*Function to display time in Analog format
*and change to the appropriate other
*main function if necessary
*********************************************/
void Clock_MainPageAnalog(void);
