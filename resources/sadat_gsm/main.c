#ifndef F_CPU
#define F_CPU 1000000UL // 16 MHz clock speed
#endif
#define D4 eS_PORTD4
#define D5 eS_PORTD5
#define D6 eS_PORTD6
#define D7 eS_PORTD7
#define RS eS_PORTC6
#define EN eS_PORTC7

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lcd.h"



void USARTInit(uint16_t ubrr_value)
{

	
	UBRRL = ubrr_value;
	UBRRH = (ubrr_value>>8);
	
	//UCSRC|=(1<<URSEL);
	UCSRC|=(1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);


	UCSRB=(1<<RXEN)|(1<<TXEN);


}
unsigned char USARTReadChar()
{


	while(!(UCSRA & (1<<RXC)));



	return UDR;
}

void USARTWriteChar(char data)
{

	while(!(UCSRA & (1<<UDRE)));
	UDR=data;
}
void USART_StringTransmit(unsigned char s[])
{
	int i=0;
	while(s[i]!='\0')
	{
		USARTWriteChar(s[i]);
		i++;
	}
}

void sendBasicMessage(){
	Lcd4_Clear();
	Lcd4_Write_String("Sending message");
	_delay_ms(2000);
	USARTInit(51);
	_delay_ms(6000);
	
	
	USARTWriteChar('A');
	_delay_ms(100);
	USARTWriteChar('T');
	_delay_ms(500);
	USARTWriteChar('\n\r');
	_delay_ms(500);
	
	
	//USARTWriteChar('A');
	//USARTWriteChar('T');
	//USARTWriteChar('+');
	//USARTWriteChar('C');
	//USARTWriteChar('M');
	//USARTWriteChar('G');
	//USARTWriteChar('F');
	//USARTWriteChar('=');
	//USARTWriteChar('1');
	//USARTWriteChar('\n\r');
	//_delay_ms(2000);
	
	USART_StringTransmit("AT+CMGF=1\n\r");
	_delay_ms(2000);
	
	
	USARTWriteChar('A');
	USARTWriteChar('T');
	USARTWriteChar('+');
	USARTWriteChar('C');
	USARTWriteChar('M');
	USARTWriteChar('G');
	USARTWriteChar('S');
	USARTWriteChar('=');
	USARTWriteChar('"');
	USARTWriteChar('+');
	USARTWriteChar('8');
	USARTWriteChar('8');
	USARTWriteChar('0');
	USARTWriteChar('1');
	USARTWriteChar('5');
	USARTWriteChar('5');
	USARTWriteChar('2');
	USARTWriteChar('4');
	USARTWriteChar('7');
	USARTWriteChar('7');
	USARTWriteChar('7');
	USARTWriteChar('3');
	USARTWriteChar('1');
	USARTWriteChar('"');
	USARTWriteChar('\n\r');
	_delay_ms(1000);
	USART_StringTransmit(" First message   ");
	USARTWriteChar(26);
	USARTWriteChar('\n\r');
	Lcd4_Clear();
	Lcd4_Write_String("Message sent");
}



int main(){
	DDRC = 0xFF;
	DDRD = 0xFF;
	Lcd4_Init();
	Lcd4_Clear();
	
	Lcd4_Set_Cursor(1,1);
	Lcd4_Write_String("GSM module");

	_delay_ms(20000);
	sendBasicMessage();
	//gsmCall();
	//makeCall();
	//call();
	while(1);
}