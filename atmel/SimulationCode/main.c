/*
 * SimulationCode.c
 *
 * Created: 19/06/2021 5:13:41 AM
 * Author : Mashiat
 */ 

#ifndef F_CPU
#define F_CPU 1000000UL // 1 MHz clock speed
#endif

#define D4 eS_PORTD4
#define D5 eS_PORTD5
#define D6 eS_PORTD6
#define D7 eS_PORTD7
#define RS eS_PORTC6
#define EN eS_PORTC7

#include <avr/io.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdbool.h>
#include <string.h>

#include "lcd.h"

int gsm_flag=0;

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
void USART_StringTransmit(char s[])
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
	
	
	USARTWriteChar('A');
	_delay_ms(100);
	USARTWriteChar('T');
	_delay_ms(500);
	USARTWriteChar('\r');
	USARTWriteChar('\n');
	_delay_ms(500);
	
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
	USARTWriteChar('7');
	USARTWriteChar('8');
	USARTWriteChar('9');
	USARTWriteChar('9');
	USARTWriteChar('4');
	USARTWriteChar('9');
	USARTWriteChar('6');
	USARTWriteChar('1');
	USARTWriteChar('5');
	USARTWriteChar('"');
	USARTWriteChar('\r');
	USARTWriteChar('\n');
	
	_delay_ms(1000);
	USART_StringTransmit(" First message   ");
	USARTWriteChar(26);
	USARTWriteChar('\r');
	USARTWriteChar('\n');
	Lcd4_Clear();
	Lcd4_Write_String("Message sent");
}


void sendMessage(char ph_string[],char turb_string[],char tds_string[])
{
		//ph=ph_string,turb=turb_string,tds=tds_string
		char msg[40] = {""};

		strcat(msg, "pH:");
		strcat(msg, ph_string);
		strcat(msg, ", Turbidity:");
		strcat(msg, turb_string);
		strcat(msg, ", TDS:");
		strcat(msg, tds_string);
		
		USARTWriteChar('A');
		_delay_ms(100);
		USARTWriteChar('T');
		_delay_ms(500);
		USARTWriteChar('\r');
		_delay_ms(500);
		USARTWriteChar('\n');
		_delay_ms(500);
		
		USART_StringTransmit("AT+CMGF=1\n\r");
		_delay_ms(2000);
		
		USART_StringTransmit("AT+CMGS=\"+8801789949615\"\r\n");
		_delay_ms(1000);
		
		USART_StringTransmit(msg);
		USARTWriteChar(26);
		
		USARTWriteChar('\r');
		USARTWriteChar('\n');
}

uint16_t read_adc() {
	uint8_t low = ADCL;
	uint16_t total = (ADCH << 8) | low;
	return total;
}

int main(void)
{
	DDRD = 0xFF;
	DDRC = 0xFF;
	DDRB = 0xFF;
	
	Lcd4_Init();
	Lcd4_Clear();
	USARTInit(51);

	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("GSM module");

	// Waiting 20s for GSM module start initializing
	// _delay_ms(20000);

	Lcd4_Clear();

	/*
		turb_string	= Digital Voltage from the Turbidity Sensor
		tds_string	= Digital Voltage from the TDS Sensor
		ph_string	= Digital Voltage from the pH Sensor
	*/
	char ph_string[5]	= "null";
	char tds_string[5]	= "null";
	char turb_string[5]	= "null";
	
	int alarm = 0;
	float sum = 0;
	PORTB = alarm;

	/*
		turb_alarm	= Alarm Flag for Turbidity Value Change
		tds_alarm	= Alarm Flag for TDS Value Change
		ph_alarm	= Alarm Flag for pH Value Change
	*/
	int ph_alarm = 0;
	int tds_alarm = 0;
	int turb_alarm = 0;
	
	/*
		temp_val	= Temperature Value
		turb_val	= Turbidity Value
		tds_val		= TDS Value
		ph_val		= pH Value
	*/
	int turb_val 	= 0;
	float ph_val 	= 0;
	float tds_val 	= 0;
	float temp_val 	= 0;

	/*
		turb_volt	= Digital Voltage from the Turbidity Sensor
		temp_volt	= Digital Voltage from the Temperature Sensor
		tds_volt	= Digital Voltage from the TDS Sensor
		ph_volt		= Digital Voltage from the pH Sensor
	*/
	float ph_volt = 0;
	float temp_volt = 0;
	float turb_volt = 0;
	
    /* Replace with your application code */
    while (1)
    {
		// ######################### Temperature from PH Sensor ######################
		// ################################### ADC - 3 ###############################

		ADMUX = 0b01000011;
		ADCSRA = 0b10000000;
		
		sum = 0;
		for(int i = 0; i < 5 ; i++){
			ADCSRA |= (1 << ADSC);
			while(ADCSRA & (1 << ADSC)){;}
			
			temp_volt = read_adc() * 5.0/1024; //milivolt??
			sum = sum + (temp_volt*10.0);
		}

		temp_val = sum/5.0;
		
		// ######################### PH Value from PH Sensor #########################
		// ############################## ADC - 1 ####################################
		// Output voltage range 0-3 Volt
		
		ADMUX = 0b01000001;
		ADCSRA = 0b10000000;
		
		sum = 0;
		for(int i = 0; i < 100 ; i++){
			ADCSRA |= (1 << ADSC);
			while(ADCSRA & (1 << ADSC)){;}
			
			ph_volt = read_adc() * 5.0/1024;
			sum += -(5.7*ph_volt) + 21.34;
		}

		ph_val = sum/100.0;
		dtostrf(ph_val, 4, 1, ph_string);

		Lcd4_Set_Cursor(1,6);
		Lcd4_Write_String("pH: ");
		Lcd4_Set_Cursor(2,6);
		Lcd4_Write_String(ph_string);

		if (ph_val < 6.5 || ph_val > 8.5) 
			ph_alarm = 1;
		else
			ph_alarm = 0;
		
		
		if ((turb_alarm == 1) || (ph_alarm == 1) || (tds_alarm == 1))
		{
			if(!alarm)
			{	
				alarm = 1;
				sendMessage(ph_string, turb_string, tds_string);
			}
		}
		else
		{
			if(alarm)
			{
				alarm = 0;
				sendMessage(ph_string, turb_string, tds_string);
			}
		}
		PORTB = alarm;
		_delay_ms(1000);
		

		// ######################### TDS Value from TDS Sensor #######################
		// ############################## ADC - 2 ####################################
		// Output Voltage Range 0-2.3 Volt

		ADMUX = 0b01000010;
		ADCSRA = 0b10000000;
		sum = 0;
		
		for (int i=0 ; i < 5 ; i++) {
			ADCSRA |= (1 << ADSC);
			while(ADCSRA & (1 << ADSC)){;}

			float raw_ec = read_adc() * 5.0/1024.0;
			float temp_coeff = 1.0 +0.02*(temp_val - 25);
			float ec = raw_ec;

			float tds = ((133.42*(ec*ec*ec)) - (255.86*ec*ec) +(857.39*ec))*0.5;
			sum += (tds/temp_coeff);
		}
		
		tds_val = sum/5.0;
		dtostrf(tds_val, 4, 0, tds_string);

		Lcd4_Set_Cursor(1,11);
		Lcd4_Write_String("TDS: ");
		Lcd4_Set_Cursor(2,11);
		Lcd4_Write_String(tds_string);
		
		if (tds_val > 1000)
			tds_alarm = 1;
		else
			tds_alarm = 0;
		
		if ((turb_alarm == 1) || (ph_alarm == 1) || (tds_alarm == 1))
		{
			if(!alarm)
			{
				alarm = 1;
				sendMessage(ph_string, turb_string, tds_string);
			}
		}
		else
		{
			if(alarm)
			{
				alarm = 0;
				sendMessage(ph_string, turb_string, tds_string);
			}
		}
		PORTB = alarm;
		_delay_ms(1000);

		// #######################  Value from Turbidity Sensor ######################
		// ############################## ADC - 4 ####################################
		// Output voltage range 0-4.5 Volt

		ADMUX = 0b01000100;
		ADCSRA = 0b10000000;
		
		sum = 0;
		int tem;
		for (int i = 0; i < 5 ; i++)
		{
			ADCSRA |= (1 << ADSC);
			while (ADCSRA & (1 << ADSC)){;}
			
			turb_volt =  read_adc() * 5.0/1024;
			tem = round(-(1120.4 * turb_volt * turb_volt) + (5742.3 * turb_volt) - 4352.9);

			if (turb_volt >= 4.2) tem = 0; 			//	4.2V or greater means clear water
			else if (turb_volt<= 2.5) tem = 3000; 	//	2.5 or lesser means too high turbidity
			sum += tem;
		}

		turb_val = sum/5;
		dtostrf(turb_val, 4, 0, turb_string);

		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("Tur: ");
		Lcd4_Set_Cursor(2, 0);
		Lcd4_Write_String(turb_string);
		
		if (turb_val > 10)
			turb_alarm = 1;
		else
			turb_alarm = 0;
		
		if ((turb_alarm == 1) || (ph_alarm == 1) || (tds_alarm == 1))
		{	
			if(!alarm)
			{	
				alarm = 1;
				sendMessage(ph_string, turb_string, tds_string);
			}
		}
		else
		{
			if (alarm)
			{
				alarm = 0;
				sendMessage(ph_string, turb_string, tds_string);
			}
		}
		PORTB = alarm;
		_delay_ms(1000);

    }
}

