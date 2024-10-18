// Net 3006 Final Project Ethan Morphew
// Main

#include <Arduino.h>
#include <avr_debugger.h>
#include <LCD.h>
#include <USART.h>
#include <ShiftReg.h>
#include <SonicSensor.h>
#include <Segment.h>

#define ALARM_BUZZER PB3
#define ALARM_LED PD3
#define MUTE_BUTTON PD2 // Triggers INT0

volatile bool alarmEnabledFlag  = true;


int main()
{
    #ifdef __DEBUG__
      dbg_start();
    #endif
	
	USART_init();
	sonicSensorInit();
	LCD_init();
	init_shift(DATA,CLOCK,LATCH);

	short int range = 0;
	byte updateCounter = 0;
	byte digits[ARRAY_SIZE_DECIMAL] = {0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBE, 0xE0, 0xFE, 0xF6 };
	char text[50];

	// Alarm Setup
	int alarmCounter = 0;
	int alarmTrigger = 0;

	DDRB |= (1 << ALARM_BUZZER);
	DDRD |= (1 << ALARM_LED);

	// Mute button setup
	DDRD &= ~(1 << MUTE_BUTTON);
	PORTD &= ~ (1 << MUTE_BUTTON); // Set pin to low
	EICRA |= (1 << ISC01 | 1 << ISC00); // INT0 triggers on rising edge
	EIMSK |= (1 << INT0); // Enable INT0

    while(1)
    {
		//Get range from u
		range = getSonicRangeCM();
		// only update the LCD and Usart output every ~1/2 second
		if(updateCounter > 50){
			updateCounter = 0;
			//LCD Update with range and Alarm setting
			sprintf(text, "Range: %d CM", range);
			LCD_command(0x01);
			LCD_string(text);
			LCD_command(0xC0);
			if(alarmEnabledFlag){
				LCD_string("Alarm is ON");
			} else {
				LCD_string("Alarm is OFF");
			}

			//Send range to USART for telemetry
			USART_send_string(text);
			USART_send('\n');

			// 7 seg displays range / 10 to simulate a center console display
			if(range < 100){
				displayValue(digits[range / 10]);
			}
			else{
				displayValue(0);
			}
		} 
		updateCounter ++;
		

		// Alarm will only beep if range is < 1m and alarm is enabled	
		if(range < 100 && alarmEnabledFlag){
			 // Alarm will beep faster as range gets smaller
			if(alarmCounter > alarmTrigger){
				alarmTrigger = range / 3;
				alarmCounter = 0;
				PORTB ^= (1 << ALARM_BUZZER);
				PORTD ^= (1 << ALARM_LED);
			}
			alarmCounter ++ ;
		}
		else {
			PORTB &= ~(1 << ALARM_BUZZER);
			PORTD &= ~(1 << ALARM_LED);
			alarmCounter = 0;
		}

		delayMs(10);
    }
}


//Toggle Alarm noise and flash using button
ISR(INT0_vect)
{
	alarmEnabledFlag = !alarmEnabledFlag;
}