#ifdef EXAMPLE_CODE

#include <avr/io.h>
#include <avr/interrupt.h>

void pause(void)
{
	int a = 0;
	for (; a < 30000; a++) PORTC ^= 1; // Flip a bit so we aren't optimized out
}

void setSID(unsigned char address, unsigned char data)
{
	PORTD  = address << 2;
	PORTA  = data;

	pause();
}

void playFreq(unsigned char freq)
{
	setSID(1,freq);
	setSID(4,21);
	pause();
	pause();
	setSID(4,22);
}

int main()
{
	DDRD  |= 0x80;			// Set PD7 for output
	TCCR2 |= (1 << WGM21);	// Clear TCNT2 on OCR2 match
	TCCR2 |= (1 << COM20);	// Toggle OC2 on match (each tick)
	TCCR2 |= (1 << CS20);	// No prescale

	OCR2   = 0;				// Match on every clock tick

	DDRA  |= 0xFF;			// SID Data port
	DDRD  |= 0xFF;			// SID Address and OSC
	DDRB  |= 0x01;			// SID Chip Select

	// Setup SID chip
	setSID(5,9);
	setSID(6,0);
	setSID(24,8);			// Max volume

	playFreq(50);
	playFreq(150);
	playFreq(10);
	playFreq(20);
	playFreq(30);
	playFreq(40);

	unsigned char b;
	for (;;)
	{
		playFreq(50);
		playFreq(150);
		playFreq(10);
		playFreq(20);
		playFreq(30);
		playFreq(40);
		playFreq(1);
		playFreq(2);
		playFreq(3);
		playFreq(4);
		pause();
	}
}

#endif
