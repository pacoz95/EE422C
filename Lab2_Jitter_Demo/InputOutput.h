//InputOutput.h
//helper functions to use port F

//Initialize all input and output pins on port F
void PortF_Init(void);

//Return PF0 and PF4 inputs
uint32_t PortF_Input(void);

//output the given data on the LEDs
void PortF_Output(uint32_t data);
