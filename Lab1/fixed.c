// filename ******** fixed.c ************** 
// Paul Cozzi and Eric Li
// 8/31/2016

#include <stdio.h>
#include <stdint.h>
#include "fixed.h"
#include "ST7735.h"

void ST7735_sDecOut3(int32_t n){
	
	if(n > 9999 || n < -9999){ //filters out invalid cases
		ST7735_OutString(" *.***");
		return;
	}
	
	if(n < 0){			//determines whether or not to print the '-' sign
		fputc('-',(FILE*)1);
		n *= -1; //gotta negate again, or you'll get negative values in the future, and weird characters print out
	}
	else{
		fputc(' ',(FILE*)1);
	}
	
	
	fputc(n/1000 + 48, (FILE*)1); //first digit
	n %= 1000;
	
	fputc('.', (FILE*)1); //decimal
	
	fputc(n/100 + 48, (FILE*)1); //second digit
	n %= 100;
	
	fputc(n/10 + 48, (FILE*)1); //third digit
	n %= 10;
	
	fputc(n + 48,(FILE*)1); //fourth digit
	
}

void ST7735_uBinOut8(uint32_t n){
	//filter out out of bounds parameters
	if(n >= 256000){
		ST7735_OutString("***.**");
	}
	//get whole number part
	uint32_t hold = (n*100) >> 8;
	
	if(hold > 9999){
		fputc(hold/10000 + 48,(FILE*)1);
		hold %= 10000;
	}
	else{
		fputc(' ',(FILE*)1);
	}
	
	if(hold > 999){
		fputc(hold/1000 + 48,(FILE*)1);
		hold %= 1000;
	}
	else{
		fputc(' ',(FILE*)1);
	}
	
	fputc(hold/100 + 48, (FILE*)1);
	hold %= 100;
	
	//print decimal point
	fputc('.',(FILE*)1);
	
	//print the decimal part
	if(hold > 0){
		fputc(hold/10 + 48, (FILE*)1);
		hold %= 10;
	}
	else{
		fputc('0',(FILE*)1);
	}
	
	if(hold > 0){
		fputc(hold + 48, (FILE*)1);
	}
	else{
		fputc('0',(FILE*)1);
	}
}
//global variables to be used for scaling
int32_t globMaxX = 0;
int32_t globMinX = 0;
int32_t globMaxY = 0;
int32_t globMinY = 0;

void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY){
	//clear the screen (fill with black) and reset the cursor
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_SetCursor(0, 0);
	
	//print the title
	ST7735_OutString(title);
	
	//clear the area for plot
	ST7735_FillRect(0,32,128,128,ST7735_WHITE); //WHITE
	
	//store the mins and maxes to be used later for scaling
	globMaxX = maxX;
	globMinX = minX;
	globMaxY = maxY;
	globMinY = minY;
}

void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[]){
	//variables to hold the scales x and y pixel coordinates
	int32_t x;
	int32_t y;
	
	//loop through the buffers
	for(int32_t k = 0; k < num; k += 1){
		x = bufX[k];
		y = bufY[k];
		
		//skip point if out of bounds
		if(x > globMaxX || x < globMinX || y > globMaxY || y < globMinY){
			continue;
		}
		//scale x
		x = 128*(x-globMinX)/(globMaxX-globMinX + 1);
		
		//scale y
		y = 32 + (128*(globMaxY - y))/(globMaxY - globMinY + 1);
		
		//print pixel
		ST7735_DrawPixel(x, y, ST7735_BLUE);
	}
}
