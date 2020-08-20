#include <lpc214x.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

void delay_ms(uint16_t j) /* Function for delay in milliseconds  */
{
    uint16_t x,i;
	for(i=0;i<j;i++)
	{
    for(x=0; x<6000; x++);    /* loop to generate 1 millisecond delay with Cclk = 60MHz */
	}
}

void LCD_CMD(char command)
{
	IO0PIN = ( (IO0PIN & 0xFFFF00FF) | (command<<8) );
	IO0SET = 0x00000040; /* EN = 1 */
	IO0CLR = 0x00000030; /* RS = 0, RW = 0 */
	delay_ms(2);
	IO0CLR = 0x00000040; /* EN = 0, RS and RW unchanged(i.e. RS = RW = 0) */
	//delay_ms(5);
}

void LCD_INIT(void)
{
	IO0DIR = IO0DIR | 0x0000FFF0; /* P0.8 to P0.15 LCD Data. P0.4,5,6 as RS RW and EN */
	delay_ms(20);
	LCD_CMD(0x38);  /* Initialize lcd */
	LCD_CMD(0x0C);   /* Display on cursor off */
	LCD_CMD(0x06);  /* Auto increment cursor */
	LCD_CMD(0x01);   /* Display clear */
	LCD_CMD(0x80);  /* First line first position */
}

void LCD_STRING (char* msg)
{
	uint8_t i=0;
	while(msg[i]!=0)
	{
		IO0PIN = ( (IO0PIN & 0xFFFF00FF) | (msg[i]<<8) );
		IO0SET = 0x00000050; /* RS = 1, , EN = 1 */
		IO0CLR = 0x00000020; /* RW = 0 */
		delay_ms(2);
		IO0CLR = 0x00000040; /* EN = 0, RS and RW unchanged(i.e. RS = 1, RW = 0) */
		//delay_ms(5);
		i++;
	}
}

void LCD_CHAR (char msg)
{
		IO0PIN = ( (IO0PIN & 0xFFFF00FF) | (msg<<8) );
		IO0SET = 0x00000050; /* RS = 1, , EN = 1 */
		IO0CLR = 0x00000020; /* RW = 0 */
		delay_ms(2);
		IO0CLR = 0x00000040; /* EN = 0, RS and RW unchanged(i.e. RS = 1, RW = 0) */
		delay_ms(5);
}

void func (float adc_read, int *tmpInt1, int *tmpInt2)
	{
  //char *tmpSign = (adc_read1 < 0) ? "-" : "";
	float tmpVal = (adc_read < 0) ? -adc_read : adc_read;
	*tmpInt1 = tmpVal;                  // Get the integer (678).
	float tmpFrac = tmpVal - *tmpInt1;      // Get fraction (0.0123).
	*tmpInt2 = tmpFrac*100;
	}  // Turn into integer (123).

int main(void)
{
	uint32_t result;
	float voltage,curr;
	//double temp_error;
	PINSEL1 = 0x01000000;
	//AD0CR = 0x00200402;	/* P0.28 as AD0.1 */
	IO0DIR = IO0DIR | (1<<1);
	IO0PIN = IO0PIN | (1<<1);
	LCD_INIT();
	//LCD_STRING("VOLTAGE");
	//LCD_CMD(0x06);
	LCD_CMD(0XC0);
	LCD_STRING("CURRENT");
	LCD_CMD(0x01);
	while(1)
	{ 
		AD0CR = 0x00200402;
		AD0CR = AD0CR | (1<<24); /* Start Conversion */
		
		while ( !(AD0GDR & 0x80000000) ){ /* Wait till DONE */
		result = AD0GDR;
		result = (result>>6);
		result = (result & 0x000003FF);
		voltage = ( (result/1023.0) * 3.3 ); /* Convert ADC value to equivalent voltage */
		curr = ((voltage-2.4575)/-64.375)*1000; 
		
	}
	  
	//char vol[18];
	char current[18];
	//float adc_read1 = voltage;
	float adc_read2 = curr;

	//char *tmpSign = (adc_read1 < 0) ? "-" : "";
	int tmpInt1;
	int tmpInt2;
	
	/*func (adc_read1, &tmpInt1, &tmpInt2);
	LCD_CMD(0x80);
  sprintf (vol, "%s%d.%02d\n", tmpSign, tmpInt1, tmpInt2);
	LCD_STRING(vol);
	LCD_CMD(0xC0);*/

  char *tmpSign2 = (adc_read2 < 0) ? "-" : "";
	func (adc_read2, &tmpInt1, &tmpInt2);
	sprintf (current, "%s%d.%02d\n", tmpSign2, tmpInt1, tmpInt2);
	LCD_STRING(current);
	LCD_STRING("mA");
	delay_ms(10);
	LCD_CMD(0x01);
     
	if (curr>=18 | curr<=4){
		IO0DIR = IO0DIR | (1<<1);
		IO0DIR = IO0DIR | (1<<16);
		IO0CLR = IO0PIN | (1<<1) | (1<<16);
		LCD_CMD(0X01);
		LCD_STRING("CRITICAL");
		LCD_CMD(0x01);
		IO0SET = IO0PIN | (1<<1);
		delay_ms(5);				}

	}	
	return 0;
}	
