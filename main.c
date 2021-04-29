#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <iom2560.h>											 // This library defines interrupt vectors
#define MaxVal 128

const char DisplayCC[10] = {0b0111111, 0b0000110,
	0b1011011, 0b1001111,
	0b1100110, 0b1101101,
	0b1111101, 0b0000111,
	0b1111111, 0b1101111};


void Timer_Setup()
{
	TCCR0A = (1<<WGM01)|(1<<COM0A1);							 // WMG01 is set as 1 to make TIMERA work in CTC. OCR0A is set (to zero) at compare match
	TCCR0B = (1<<CS00);											 // Prescaler=0
	OCR0A=MaxVal;												 // MaxVal=128. OCR0A is the register used to compare TCNT0 counter register.
}

void Ports_Setup()
{
	DDRA = (1<<PA7)|(1<<PA6)|(1<<PA5)							// Digital pins 29 to 27 are set as outputs (1) to multiplex display's GND
	DDRD = (1<<PB1)|(1<<PB0);									// Digital pins 53 and 52 (PB0 and PB1) are set as input (0)
	DDRC = 0xFF;												// C ports (digital pins 37 to 30) are set as outputs (1)
	EICRA = (1<<ISC11)|(1<<ISC10)|(1<<ISC01)|(1<<ISC00)			// Rising edge of INT0 and INT1 generate an interrupt request
	EIMSK = (1<<INT1)|(1<<INT0)									// Interrupt mask for INT0 and INT1 disabled
	EIFR = 0xFC;												// Flags for INT0 and INT1 disabled
}

int main(void)
{
	Ports_Setup();
	Timer_Setup();
	sei();

	while (1){}
}

ISR(INT0_vect)
{
	
}

ISR(INT1_vect)
{
	
}

ISR(TIMER0_COMPA_vect)
{
	
	_delay_ms(100);	
}
