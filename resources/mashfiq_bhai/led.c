//**************************************************************//
//Microcontroller			:ATmega32
//System Clock				:1MHz
//Project					:MQ-2 Smoke Sensor Interfacing with ATmega32 and LCD Display
//Software					:ATmega Studio 6
//LCD Data Interfacing		:8-Bit
//Author					:Arun Kumar Garg
//							:ABLab Solutions
//							:www.ablab.in
//							:info@ablab.in
//Date						:1st august 2015
//**************************************************************//

#include<avr/io.h>
/*Includes io.h header file where all the Input/Output Registers and its Bits are defined for all AVR microcontrollers*/

#define	F_CPU	1000000
/*Defines a macro for the delay.h header file. F_CPU is the microcontroller frequency value for the delay.h header file. Default value of F_CPU in delay.h header file is 1000000(1MHz)*/

#include<util/delay.h>
/*Includes delay.h header file which defines two functions, _delay_ms (millisecond delay) and _delay_us (microsecond delay)*/

#define		LCD_DATA_PORT		PORTB
/*LCD_DATA_PORT is the microcontroller PORT Register to which the data pins of the LCD are connected. Here it is connected to PORTB*/

#define 	LCD_CONT_PORT		PORTA
/*LCD_CONT_PORT is the microcontroller PORT Register to which the control pins of the LCD are connected. Here it is connected to PORTD*/

#define 	LCD_RS 		PA5
/*LCD_RS is the microcontroller Port pin to which the RS pin of the LCD is connected. Here it is connected to PD0*/

#define 	LCD_RW 		PA6
/*LCD_RW is the microcontroller Port pin to which the RW pin of the LCD is connected. Here it is connected to PD1*/

#define 	LCD_EN 		PA7
/*LCD_EN is the microcontroller Port pin to which the EN pin of the LCD is connected. Here it is connected to PD2*/

#include<stdlib.h>
/*Includes stdlib.h header file which defines different standard C functions*/


#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>



#define SREG   _SFR_IO8(0x3F)

void GSM_Begin();
void GSM_Calling(char *);
void GSM_HangCall();
void GSM_Response();
void GSM_Response_Display();
void GSM_Msg_Read(int);
bool GSM_Wait_for_Msg();
void GSM_Msg_Display();
void GSM_Msg_Delete(unsigned int);
void GSM_Send_Msg(char* , char*);
void GSM_Delete_All_Msg();


char buff[160];								/* buffer to store responses and messages */
char status_flag = 0;						/* for checking any new message */
volatile int buffer_pointer;
char Mobile_no[]="+8801683267750";							/* store mobile no. of received message */
char message_received[60];					/* save received message */
int position = 0;							/* save location of current message */
char mmm[]="Fire! Fire! call 2222.";




void USART_Init(unsigned long BAUDRATE)				/* USART initialize function */
{
	int BAUD_PRESCALE=(((F_CPU / (BAUDRATE * 16UL))) - 1);
	UCSRB |= (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);				/* Enable USART transmitter and receiver */
	UCSRC |= (1 << URSEL)| (1 << UCSZ0) | (1 << UCSZ1);	/* Write USCRC for 8 bit data and 1 stop bit */
	UBRRL = BAUD_PRESCALE;							/* Load UBRRL with lower 8 bit of prescale value */
	UBRRH = (BAUD_PRESCALE >> 8);					/* Load UBRRH with upper 8 bit of prescale value */
	//PORTA=0XFF;
}

char USART_RxChar()									/* Data receiving function */
{
	while (!(UCSRA & (1 << RXC)));					/* Wait until new data receive */
	return(UDR);									/* Get and return received data */
}

void USART_TxChar(char data)						/* Data transmitting function */
{
	UDR = data;										/* Write data to be transmitting in UDR */
	while (!(UCSRA & (1<<UDRE)));					/* Wait until data transmit and buffer get empty */
	//PORTA=0XFF;
}

void USART_SendString(char *str)					// Send string of USART data function */
{

	int i=0;
	while (str[i]!=0)
	{
		USART_TxChar(str[i]);						/* Send each char of string till the NULL */
		i++;
	}
}











/*ADC Function Declarations*/
void adc_init(void);
int read_adc_channel(unsigned char channel);

/*Alphanumeric LCD Function Declarations*/
void lcd_data_write(char data);
void lcd_command_write( char command);
void lcd_init();
void lcd_string_write( char *string);
void lcd_number_write(int number,unsigned char radix);

int main(void)
{
	DDRB=0xff;
	DDRC=0xFF;
	/*All the 8 pins of PortB are declared output (data pins of LCD are connected)*/

	DDRA=0b11100000;
	/*PD0, PD1 and PD2 pins of PortD are declared output (control pins of LCD are connected)*/
	DDRD |= 0b00010000;
	int mq2_smoke_sensor_output;
	/*Variable declarations*/
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	USART_Init(9600);
	sei();
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	lcd_init();
	/*LCD initialization*/

	adc_init();
	/*ADC initialization*/

	lcd_string_write("ABLab Solutions");
	/*String display in 1st row of LCD*/

	lcd_command_write(0xc0);
	/*Cursor moves to 2nd row 1st column of LCD*/

	lcd_string_write("www.ablab.in");
	/*String display in 2nd row of LCD*/

	_delay_ms(500);
	_delay_ms(500);
	_delay_ms(500);
	_delay_ms(500);
	/*Display stays for 2 second*/

	/*Start of infinite loop*/
	while(1)
	{
		lcd_command_write(0x01);
		/*Clear screen*/

		lcd_string_write("MQ-2 Value:-");
		/*String display in 1st row of LCD*/

		lcd_command_write(0xc0);
		/*Cursor moves to 2nd row 1st column of LCD*/

		mq2_smoke_sensor_output=read_adc_channel(0);
		/*Reading MQ-2 Smoke Sensor output of channel 0*/

		lcd_number_write(mq2_smoke_sensor_output,10);
		/*MQ-2 Smoke Sensor output value is displayed in 2nd row of LCD*/

		_delay_ms(500);
		/*Display stays for 500ms*/

		//press buzzer
		if(mq2_smoke_sensor_output >= 300){
			PORTD |= 0b00010000;//buzzer pressed
			_delay_ms(500);
			GSM_Send_Msg(Mobile_no,mmm);
			while(1){
				DDRC &= 0b01111111;
				
				if(PINC & 0b10000000){//switch pressed to off buzzer and re start adc
					PORTD &= 0b11101111;
					_delay_ms(500);
					break;
				}
			}
		}
//		else{
//			PORTD &= 0b11101111;
//		}
	}
}
/*End of Program*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



ISR(USART_RXC_vect)
{
	buff[buffer_pointer] = UDR;							/* copy UDR(received value) to buffer */
	buffer_pointer++;
	status_flag = 1;						            /* flag for new message arrival */
}



void GSM_Send_Msg(char *num,char *sms)
{
	char sms_buffer[35];
	buffer_pointer=0;
	sprintf(sms_buffer,"AT+CMGS=\"%s\"\r",num);

	USART_SendString(sms_buffer);                       /*send command AT+CMGS="Mobile No."\r */

	_delay_ms(200);
	while(1)
	{
		if(buff[buffer_pointer]==0x3e)                  /* wait for '>' character*/
		{
			buffer_pointer = 0;
			memset(buff,0,strlen(buff));
			USART_SendString(sms);                      /* send msg to given no. */
			USART_TxChar(0x1a);                         /* send Ctrl+Z then only message will transmit*/
			break;
		}
		buffer_pointer++;
	}
	_delay_ms(300);
	buffer_pointer = 0;
	memset(buff,0,strlen(buff));
	memset(sms_buffer,0,strlen(sms_buffer));
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




/*ADC Function Definitions*/
void adc_init(void)
{
	ADCSRA=(1<<ADEN)|(1<<ADSC)|(1<<ADATE)|(1<<ADPS2);
	SFIOR=0x00;
}

int read_adc_channel(unsigned char channel)
{
	int adc_value;
	unsigned char temp;
	ADMUX=(1<<REFS0)|channel;
	_delay_ms(1);
	temp=ADCL;
	adc_value=ADCH;
	adc_value=(adc_value<<8)|temp;
	return adc_value;
}

/*Alphanumeric LCD Function Definitions*/
void lcd_data_write(char data)
{
	LCD_CONT_PORT=_BV(LCD_EN)|_BV(LCD_RS);
	LCD_DATA_PORT=data;
	_delay_ms(1);
	LCD_CONT_PORT=_BV(LCD_RS);
	_delay_ms(1);
}

void lcd_command_write(char command)
{
	LCD_CONT_PORT=_BV(LCD_EN);
	LCD_DATA_PORT=command;
	_delay_ms(1);
	LCD_CONT_PORT=0x00;
	_delay_ms(1);
}

void lcd_init()
{
	lcd_command_write(0x38);
	lcd_command_write(0x01);
	lcd_command_write(0x06);
	lcd_command_write(0x0e);
}

void lcd_string_write(char *string)
{
	while (*string)
	lcd_data_write(*string++);
}

void lcd_number_write(int number,unsigned char radix)
{
	char *number_string="00000";
	itoa(number,number_string,radix);
	lcd_string_write(number_string);
}

