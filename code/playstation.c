/******************************
* playstation.c
*  Created on:	18-Feb-2010
*  Author:		Edwin Amsler
******************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "playstation.h"

Controller readController()
{
	Controller output;

	PORTB &= ~(1 << PB4);	// PlayStation attention line low. Controller is active low
	PlayStationComm(0x01);	// Out: Begin				In: None
	PlayStationComm(0x42);	// Out: Issue Read data 	In: Device mode [7-4], Payload length [3-0]
	PlayStationComm(0xFF);	// Out: Ignored				In: Always 0xA5. Unknown purpose

	output.bytes[0] = ~PlayStationComm(0x00);
	output.bytes[1] = ~PlayStationComm(0x00);
	output.bytes[2] = ~PlayStationComm(0x00);
	output.bytes[3] = ~PlayStationComm(0x00);
	output.bytes[4] = ~PlayStationComm(0x00);
	output.bytes[5] = ~PlayStationComm(0x00);
	PORTB |= (1 << PB4);	// PlayStation attention line high

	PlayStationComm(0x00);	// Throw in some delay for now. Can't change
							// from digital to analog without it

	return output;
}

// Example of polling controller for data.
#ifdef EXAMPLE
void sample()
{
	PlayStationInit();

	for (;;)
	{
		PORTB &= ~(1 << PB4);		// PlayStation attention line low

		PlayStationComm(0x01);	// Out: Begin				In: None
		PlayStationComm(0x42);	// Out: Issue Read data 	In: Device mode [7-4], Payload length [3-0]
		PlayStationComm(0xFF);	// Out: Ignored				In: Always 0xA5. Unknown purpose

		PORTD = PlayStationComm(0x00);	// Out: Motor control		In: Sel,  L3,  R3, Str,  Up,   R, Dwn, Lft
		PlayStationComm(0x00);	// Out: Motor control		In:  L2,  R2,  L1,  R1, Tri,   O,   X, Sqr

		PlayStationComm(0x00);	// Out: Ignored				In: Right	Analog X
		PlayStationComm(0x00);	// Out: Ignored				In: Right	Analog Y
		PlayStationComm(0x00);	// Out: Ignored				In: Left	Analog X
		PlayStationComm(0x00);	// Out: Ignored				In: Left	Analog Y

		// Controller needs to be specially configured for these
		/*
		PlayStationComm(0xFF);	// Out: Ignored				In: Right	Pressure
		PlayStationComm(0xFF);	// Out: Ignored				In: Left	Pressure
		PlayStationComm(0xFF);	// Out: Ignored				In: Up		Pressure
		PlayStationComm(0xFF);	// Out: Ignored				In: Triangl	Pressure
		PlayStationComm(0xFF);	// Out: Ignored				In: Circle	Pressure
		PlayStationComm(0xFF);	// Out: Ignored				In: X		Pressure
		PlayStationComm(0xFF);	// Out: Ignored				In: Square	Pressure
		PlayStationComm(0xFF);	// Out: Ignored				In: L1		Pressure
		PlayStationComm(0xFF);	// Out: Ignored				In: R1		Pressure
		PlayStationComm(0xFF);	// Out: Ignored				In: L2		Pressure
		PlayStationComm(0xFF);	// Out: Ignored				In: R2		Pressure
		*/

		PORTB |= (1 << PB4);		// PlayStation attention line high

		PlayStationComm(0x00);		// Throw in some delay for now. Can't change
									// from digital to analog without it
	}
}
#endif

void spiInit()
{
	// Default speed of PlayStation SPI bus is 250KHz, which didn't work for me. Slower allowed stable results
	// Turns out during the presentation the whole thing fell apart. Reason was due to lack of a real pull-up
	// resistor. Adding that spare resistor not only saved my presentation at zero hour, but should allow much
	// higher sample rates
	DDRB  |= (1 <<  PB4) | (1 <<  PB5) | (1 << PB7);	// Set SPI out, clock and slave select for output
	SPCR   = (1 <<  SPE) | (1 << MSTR);					// Enable SPI, set to master mode
	SPCR  |= (1 << CPOL) | (1 << CPHA);					// Set on fall, sample on rising
	SPCR  |= (1 << DORD);								// LSB first. Derp! Lots of trial and error to find that out

	SPCR  |= (1 << SPR0) | (1 << SPI2X);				// SPI clock. Should be running at 250KHz (FOSC / 8)
}

void PlayStationInit()
{
	spiInit();
}

// Transmit and recieve one byte
unsigned char PlayStationComm(unsigned char byte)
{
	SPDR = byte;               							// Load byte to Data register
	while(!(SPSR & (1<<SPIF)));    						// Busy wait for finish
	return SPDR;										// Spit back what was recieved
}
