/**************************************************
	Commodore SID chip control code (mos8580.c)
	Author:	Edwin Amsler <edwinguy@gmail.com>
	Date:	March 30, 2010

**************************************************/

#include <avr/io.h>
#include "mos8580.h"
#include "noritake_vfd.h"

void delay()
{
	unsigned short int a;
	for (a = 0; a < 10; a++) PORTC ^= 1;
}

// Initialize hardware to talk to the SID chip.
void SIDInit()
{
	// Create an external 1MHz clock pulse on OC2
	TCCR2 |= (1 << WGM21);	// Clear TCNT2 on OCR2 match
	TCCR2 |= (1 << COM20);	// Toggle OC2 on match (each tick)
	TCCR2 |= (1 << CS20);	// No pre-scale

	OCR2   = 0;				// Match on every clock tick

	SID_DATA_DIR  |= 0xFF;	// SID Data port
	SID_ADDR_DIR  |= 0xFF;	// SID Address and OSC

	// Zero all registers. SID really doesn't like being reset in hardware
	unsigned char loop = 0;
	for (; loop < ENV3; loop++) // ENV3 is the last register of the SID
	{
		SIDSet(loop, 0);
	}
}

void SIDSet(unsigned char address, unsigned char data)
{
	SID_ADDR = address << 2;	// Shift since we aren't using the whole
	SID_DATA = data;			// data port.
	delay();					// Quick delay to let the SID sample properly 
}
