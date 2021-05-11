#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define MaxVal 128

const enum fsmStates {zero, start, stop, lap} fsm;
volatile enum fsmStates fsmState = zero;						 // fsmState is declared as "volatile" because it's a global variable modified by an ISR.
volatile float counter = 0;

const char DisplayCC[10] = {0b0111111, 0b0000110,
	0b1011011, 0b1001111,
	0b1100110, 0b1101101,
	0b1111101, 0b0000111,
0b1111111, 0b1101111};


void Timer_Setup()
{
	TCCR0A = (1<<WGM01)|(1<<COM0A1);							// WMG01 is set as 1 to make TIMERA work in CTC. OCR0A is set (to zero) at compare match
	OCR0A=MaxVal;												// MaxVal=128. OCR0A is the register used to compare TCNT0 counter register.
}

void System_Init()
{
	DDRA = (1<<PA7)|(1<<PA6)|(1<<PA5);							// Digital pins 29 to 27 are set as outputs (1) to multiplex display's GND
	DDRD = (0<<PB1)|(0<<PB0);									// Digital pins 53 and 52 (PB0 and PB1) are set as input (0)
	DDRC = 0xFF;												// C ports (digital pins 37 to 30) are set as outputs (1)
	EICRA = (1<<ISC11)|(1<<ISC10)|(1<<ISC01)|(1<<ISC00);		// Rising edge of INT0 and INT1 generate an interrupt request
	EIMSK = (1<<INT1)|(1<<INT0);								// Interrupt mask for INT0 and INT1 disabled
	EIFR = 0xFC;												// Flags for INT0 and INT1 disabled
}

void Led_Output(float input)
{
	
}

int main(void)
{
	System_Init();
	Timer_Setup();
	sei();

	while (1) {}
}

ISR(INT0_vect)
{
	if(fsmState==zero)
	{
		fsmState=start;
		TCCR0B = (1<<CS00);										// Prescaler=0, counter starts
	}																
	else
	{
		if (fsmState==start)
		{
			fsmState=stop;
			TCCR0B&=0x00;										// Counter is stopped
		}
	}
}

ISR(INT1_vect)
{
	if (fsmState==stop)
	{
		fsmState=zero;
		TCNT0=0x00;
		counter=0;
	}
	else
	{
		if (fsmState==start)
		{
			fsmState=lap;
		}
		else
		{
			if (fsmState==lap)
			{
				fsmState=start;
			}
		}
	}
}

ISR(TIMER0_COMPA_vect)
{
	counter=counter+0.1;
	_delay_ms(100);
	if (counter>=60.0)
	{
		counter=counter-60.0;
	}
	if (fsmState!=lap)
	{
		Led_Output(counter);									// Displays actual count
	}
}
