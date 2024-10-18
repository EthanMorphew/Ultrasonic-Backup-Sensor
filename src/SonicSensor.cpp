// Net 3006 Final Project Ethan Morphew

#include <SonicSensor.h>
#include <USART.h>

volatile static bool PULSE_FLAG; //Pulse becomes high upon interrupt PCINT0_vect detecting a leading edge and low upon detecting trailing edge
volatile static unsigned int PULSE_COUNT; //stores number of timer ticks taken for return of pulse

void sonicSensorInit(){
    // Trigger is output Echo is input
    DDRB |= (1 << TRIGGER);
	DDRB &= ~ (1 << ECHO);
	// Pin Change Interrupt
	PCICR |= (1 << PCIE0);
	PCMSK0 |= (1 << PCINT5);
	sei();
    // Timer off, here for clarity
	TCCR1B &= ~(1 << CS10);
}

int getSonicRangeCM(){
    PORTB |= (1 << TRIGGER);
	_delay_us(12);
	PORTB &= ~(1 << TRIGGER);
	//distance = (speed of sound in m/s) * (timer tick for 64 prescalar) * (timer count) * (100 to convert to cm) / (2 as the sonar pulse must travel to the object and back)
	int distance = 343 * (0.000004 * double(PULSE_COUNT) * 100) / 2;
	return distance;
}

// Using ECHO pin (PB5) gets number of clock cycles for a pulse from ECHO
ISR(PCINT0_vect){
	if(!PULSE_FLAG){
		TCCR1B |= (1 << CS10 | 1 << CS11);//Start timer with 64 prescaler
		TCNT1 = 0;
		PULSE_FLAG = true;
	}
	else {
		TCCR1B &= ~(1 << CS10 | 1 << CS11);//Stop timer
		PULSE_COUNT = TCNT1;
		PULSE_FLAG = false;
	}
}