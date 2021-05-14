#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define MaxVal 128;

const enum fsmStates {zero, start, stop, lap} fsm;
volatile enum fsmStates fsmState = zero;						 // fsmState is declared as "volatile" because it's a global variable modified by an ISR.
volatile float counter = 0;
volatile float auxcounter = 0;
const float ExperimentalDelay = 62.434;

const uint8_t DisplayCC[10] = {0b11111110, 0b10110000,
	0b11101101, 0b11111001,
	0b10110011, 0b11011011,
	0b11011111, 0b11110000,
    0b11111111, 0b11110011};
const uint8_t* pDisplayCC = DisplayCC;

void Timer_Setup()
{
	TCCR0A = (1<<WGM01)|(1<<COM0A1);							// WMG01 is set as 1 to make TIMERA work in CTC. OCR0A is set (to zero) at compare match
	TIMSK0 = (1<<OCIE0A);										// Interrupt mask disabled for compare to match OCR0A.
	OCR0A=MaxVal;												// MaxVal=128. OCR0A is the register used to compare TCNT0 counter register.
}

void System_Init()
{
	DDRA = (1<<PA7)|(1<<PA6)|(1<<PA5);							// Digital pins 29 to 27 are set as outputs (1) to multiplex display's GND
	DDRD &= ~(1<<PD1)|(1<<PD0);									// Digital pins 21 and 20 (PD0 and PD1) are set as input (0)
	DDRC = 0xFF;												// C ports (digital pins 37 to 30) are set as outputs (1)
	EICRA = (1<<ISC11)|(1<<ISC10)|(1<<ISC01)|(1<<ISC00);		// Rising edge of INT0 and INT1 generate an interrupt request
	EIMSK = (1<<INT1)|(1<<INT0);								// Interrupt mask for INT0 and INT1 disabled
}

void Led_Output(float input)
{
	uint8_t decimal=input, tens=input/10, units, pdecimal= 0b01111111;
	decimal=10*(input-decimal);
	units=input-10*tens;
	
	// Decimal digit output
	PORTA=0b01111111;
	PORTC=(*(pDisplayCC+decimal) & pdecimal);
	_delay_ms(1);

	PORTC=0b00000000;
	// Unit digit output
	PORTA=0b10111111;
	PORTC=*(pDisplayCC+units);
	_delay_ms(1);

	if (tens>0)
	{
		PORTC=0b00000000;
		// Tens digit output
		PORTA=0b11011111;
		PORTC=(*(pDisplayCC+tens) & pdecimal);
	}
	_delay_ms(1);
}

int main(void)
{
	System_Init();
	Timer_Setup();
	sei();
	
	while (1) 
	{
		if ((fsmState==start) | (fsmState==lap))
		{
			counter=counter+0.1;
			_delay_ms(ExperimentalDelay);						// Delay to sync the counter.
			if (counter>=60.0)
			{
				counter=0;
			}
		}
	}
}

ISR(INT0_vect)
{
	if(fsmState==zero)
	{
		fsmState=start;
		TCCR0B = (1<<CS02)|(1<<CS00);							// Prescaler=1024, counter starts
	}																
	else
	{
		if (fsmState==start)
		{
			fsmState=stop;
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
			auxcounter=counter;
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
	if (fsmState!=lap)
	{
		Led_Output(counter);								// Displays actual count
	}
	else 
	{
		Led_Output(auxcounter);								// Displays count before lap state
	}
}
