/*
 * SimulationCode.c
 *
 * Created: 19/06/2021 5:13:41 AM
 * Author : Mashiat
 */ 


#define F_CPU 1000000
#define D4 eS_PORTD4
#define D5 eS_PORTD5
#define D6 eS_PORTD6
#define D7 eS_PORTD7
#define RS eS_PORTC6
#define EN eS_PORTC7

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include "lcd.h"


int main(void)
{
	DDRD = 0xFF;
	DDRC = 0xFF; 
	DDRB = 0xFF;
	
	//int msb, i;
	char strpH[5];
	char strTDS[5];
	char strTur[5];
	Lcd4_Init();
	
	int alarm = 0;
	PORTB = alarm;
	
	float sum = 0;
	
    /* Replace with your application code */
    while (1) 
    {
		
		
		ADMUX = 0b01100011;
		ADCSRA = 0b10000000;
		
		sum = 0;
		
		for(int i = 1; i<5 ; i++){
			ADCSRA |= (1<<ADSC);
			
			while(ADCSRA & (1<<ADSC)){;}
			
			float tempvoltage =  (ADCH<<2)*5.0/1024; //milivolt??
			sum = sum + (tempvoltage*10.0);
		}
		float temp = sum/5.0;
		
		//comment out later and delete
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("Temp: ");
		dtostrf(temp, 4, 1, strTur);
		
		//dtostrf(Turvolt, 4, 2, strTur);
		Lcd4_Set_Cursor(2, 0);
		Lcd4_Write_String(strTur);
		_delay_ms(1000);
		
		/*
		
		//-------------------------Turbidity Sensor - A0---------------------------
		//output voltage range - 0-4.5V
		ADMUX = 0b01100000;
		ADCSRA = 0b10000000;
		
		ADCSRA |= (1<<ADSC);
		
		while(ADCSRA & (1<<ADSC)){;}
		
		
		float Turvolt =  (ADCH<<2)*5.0/1024;
		int Turbidity =round(-(1120.4*Turvolt*Turvolt) + (5742.3*Turvolt) - 4352.9);
		if(Turvolt >= 4.2) Turbidity = 0; //4.2V or greater means clear water
		else if(Turvolt<= 2.5) Turbidity = 3000; //2.5 or lesser means too high turbidity
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("Tur:");
		
		dtostrf(Turbidity, 4, 0, strTur);
		
		//dtostrf(Turvolt, 4, 2, strTur);
		Lcd4_Set_Cursor(2, 0);
		Lcd4_Write_String(strTur);
		
		//Alarm
		//check Turbidity level - 10 NTU
		if(Turbidity>10){
			alarm = alarm|1;
		}
		else{
			alarm =alarm&254;
		}
		PORTB = alarm;
		
		*/
		//------------------ FOR pH - A1 --------------------------------
		//output voltage range  = 0-3V
		
		
		ADMUX = 0b01100001;
		ADCSRA = 0b10000000;
		
		sum = 0;
		
		for(int i = 0; i<5 ; i++){
			ADCSRA |= (1<<ADSC);
			
			while(ADCSRA & (1<<ADSC)){;}
			
			//taking aref as = 4.5 V
			float pHvoltage =  (ADCH<<2)*5.0/1024;
			//float pH = -(5.7*pHvoltage) + 21.34;
			//float pH = 3.5*pHvoltage;
			sum += -(3.57*pHvoltage)+21.35;
		}
		float pH = sum/5.0;
		
		Lcd4_Set_Cursor(1,6);
		Lcd4_Write_String("pH:  ");
		
		//dtostrf(pH, 4, 1, strpH);
		dtostrf(pH, 4, 1, strpH);
		Lcd4_Set_Cursor(2,6);
		Lcd4_Write_String(strpH);
		_delay_ms(1000);
		
		//Alarm
		//check pH level - 6.5-8.5
		/*
		if(pH<6.5 || pH>8.5){ alarm =alarm|2;}
		else{ alarm = alarm&253; }
			
		PORTB = alarm;
		
		*/
		
		
		//----------------Temperature from pH - A3--------------------------------
		
		/*
		ADMUX = 0b00100011;
		ADCSRA = 0b10000000;
		
		ADCSRA |= (1<<ADSC);
		
		while(ADCSRA & (1<<ADSC)){;}
		
		float tempvoltage =  (ADCH<<2)*5/1024.0; //milivolt??
		//float temp = tempvoltage*0.1;
		
		*/
		
		
		//------------------------------TDS -- A2-----------------------------
		//output voltage range = 0-2.3
		ADMUX = 0b01100010;
		ADCSRA = 0b10000000;
		sum = 0;
		
		for(int i = 0 ; i<5 ; i++){
			ADCSRA |= (1<<ADSC);
			
			while(ADCSRA & (1<<ADSC)){;}
			
			//taking aref as = 4.5 V
			float rawEC =  (ADCH<<2)*5.0/1024.0;
			float tempCoeff = 1.0 +0.02*(temp - 25);
			//float ec = (rawEC/tempCoeff);
			float ec = rawEC;
			//:tds = (133.42 * pow(sensor::ec, 3) - 255.86 * sensor::ec * sensor::ec + 857.39 * sensor::ec) * 0.5;
			float tds = ((133.42*(ec*ec*ec)) - (255.86*ec*ec) +(857.39*ec))*0.5;
			sum += (tds/tempCoeff);
		}
		
		float tds = sum/5.0;
		Lcd4_Set_Cursor(1,11);
		Lcd4_Write_String("TDS: ");
		
		dtostrf(tds, 4, 0, strTDS);
		//dtostrf(rawEC, 4, 2, strTDS);
		Lcd4_Set_Cursor(2,11);
		Lcd4_Write_String(strTDS);
		_delay_ms(1000);
		/*
		//Alarm
		//check TDS level - 1000
		if(tds>1000){ 
			alarm = alarm|4;
		}
		else{ 
			alarm = alarm&251; 
		}
		PORTB = alarm;
		*/
		
    }
}

