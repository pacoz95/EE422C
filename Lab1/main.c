// filename ******** fixed.c ************** 
// provides a way to test the four functions written for the lab
// Paul Cozzi and Eric Li
// 8/31/2016
#include <stdio.h>
#include <stdint.h>
#include "PLL.h"
#include "ST7735.h"
#include "fixed.h"
void testDecOut(){
	for(int32_t k = -100; k < 100; k += 1){
		ST7735_sDecOut3(k);
		fputc('\n',(FILE*)1);
	}
}
int main(void){
	PLL_Init(Bus80MHz);                  // set system clock to 80 MHz
  ST7735_InitR(INITR_REDTAB);
	//testDecOut();
	ST7735_uBinOut8(512);
	ST7735_XYplotInit("Title", 0, 0, 0, 0);
	return 0;
}
