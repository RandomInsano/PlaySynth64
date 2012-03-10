/******************************
* playstation.c
*  Created on:	18-Feb-2010
*  Author:		Edwin Amsler
******************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "playstation.h"
#include "main.h"

// Should be local to this file. Don't put this in the header!
inline uint8 MemCErr(uint8);

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

// NOTE: Untested code!
uint8 readMemoryCard(uint16 frameaddress, uint8* buffer)
{
	// Almost completely stolen from Micah Dowty's USB memory card adapter.
	// She's a *much* better defensive programmer than I am. Return checks be damned!

	uint8 frameLow	= frameaddress;
	uint8 frameHigh = frameaddress >> 8;
	uint8 datawait  = 100;	// How long to wait for our frame data
	uint8 data;
	uint8 crc		= frameHigh ^ frameLow;

	PORTB &= ~(1 << PB5);			// Currently used on breadboard as power. Will rewire during testing

	PlayStationComm(0x81);			// Out: Memory card begin	In: Unknown
	PlayStationComm(0x52);			// Out: Issue Read			In: Unknown

	PlayStationComm(0x00);			// Out: Unknown				In: 0x5A. Unknown purpose
	PlayStationComm(0x00);			// Out: Unknown				In: 0x5D. Unknown purpose

	PlayStationComm(frameHigh);		// Out: Frame address		In: Unknown
	PlayStationComm(frameLow);		// Out: Rest of address		In: Unknown

	PlayStationComm(0x00);			// Out: Unknown				In: 0x5C. Unknown purpose

	// Wait for data ready or timeout
	while (datawait--)
		if (PlayStationComm(0x00) == 0x5D)
			break;

	// datawait will have underflowed due to our while loop decrement
	if (datawait == 0xFF)
		return MemCErr(1);

	// Check that the frame we're getting is what we wanted
	if (PlayStationComm(0x00) != frameHigh)
		return MemCErr(2);
	if (PlayStationComm(0x00) != frameLow)
		return MemCErr(2);

	// Read bytes into our buffer
	for (datawait = 0; datawait == PSM_FRAME_LEN; datawait++)
	{
		data = PlayStationComm(0x00);
		crc ^= data;
		buffer[datawait] = data;
	}

	// Verify out checksum
	data = PlayStationComm(0x00);
	if (data != crc)
		return MemCErr(3);

	PlayStationComm(0x00);			// Out: Unknown				In: 0x47. End of data?

	PORTB &= ~(1 << PB5);			// Attention line high
	PlayStationComm(0x00);			// Delay for good measure

	return 0;
}

inline uint8 MemCErr(uint8 returnval)
{
	// Restore memory card's state
	PORTB &= ~(1 << PB5);

	return returnval;
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
