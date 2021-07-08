/*
	LCD Printing test file
*/

#define D4 eS_PORTD4
#define D5 eS_PORTD5
#define D6 eS_PORTD6
#define D7 eS_PORTD7
#define RS eS_PORTC6
#define EN eS_PORTC7

#include <avr/io.h>
#include "lcd.h"


int main(void)
{
	DDRD = 0xFF;
	DDRC = 0xFF;
    /*
        ADMUX   = REFS1 REFS0 ADLAR MUX(4-0)
                  0     0     0     0 0 0 0 1
        ADCSRA  = ADEN ADSC ADATE ADIF ADIE ADPS(2-0)
                  1     ?     1     ?   ?     1 0 0
    */
    ADMUX  = 0b00000001;
    ADCSRA = 0b10000100;

	Lcd4_Init();
	char voltage_string[16] = {"hey, there"};
	
	while(1)
	{	
		Lcd4_Set_Cursor(1,1);
		Lcd4_Write_String(voltage_string);
	}
	
	return 0;
}
