/***************************************************
* main.c2
*  Example showing how to use
*  the noritake library thing.
*
*  Playstation comms fails here, I'm leaving it
*  around for historical purposes.
*
*  Created on:	25-Feb-2010
*  Author:		Edwin Amsler
***************************************************/

#include <avr/io.h>
#include "noritake_vfd.h"
#include "playstation.h"
#include "mos8580.h"
#include "settings.h"

const unsigned short int notes[13][2] =
{
		{225, 'A'},
		{239, 'A' | '#' << 8},
		{253, 'B'},
		{268, 'C'},
		{284, 'C' | '#' << 8},
		{301, 'D'},
		{319, 'D' | '#' << 8},
		{338, 'E'},
		{358, 'F'},
		{379, 'F' | '#' << 8},
		{402, 'G'},
		{425, 'G' | '#' << 8},
		{0,  '\0'},
};

#define FOSC	2000000	// Clock Speed
#define BAUD	   9600	// Desired baud rate

#define byte	unsigned char
#define uint16  unsigned short int

void printbyte(byte output)
{
	print("D[");
	printnum(output);
	print("] ");
}

int main(void)
{
	// Initialize devices
	PlayStationInit();
	VFDInit(FOSC / 16 / BAUD - 1);	// Configure display
	SIDInit();						// Configure sound
	LoadConfig();					// Load defaults from eeprom

	uint16 frequency;
	uint16 oldFrequency = 0;
	uint16 note;
	byte   octave;
	byte   noteIndex;

	println("Play mode:");

	// Main program loop
	for (;;)
	{
		Controller control = readController();

		// R1, L2, and L1 are used to select octaves, so ignore them
		// as notes. R2 is used to specify a sharp.
		// Ignore those buttons for figuring out the note 
		switch (control.buttons & ~(PS_L1 | PS_L2 | PS_R1 | PS_R2))
		{
			case PS_START:		// Enter configuration menu
				configure();

				// Restore interface
				println("Play mode:");
				noteIndex = 12;
				break;

			case PS_UP:			// A
				noteIndex = 0;
				break;

			case PS_LEFT:		// B
				noteIndex = 2;
				break;

			case PS_DOWN:		// C
				noteIndex = 3;
				break;

			case PS_RIGHT:		// D
				noteIndex = 5;
				break;

			case PS_TRIANGLE:	// E
				noteIndex = 7;
				break;

			case PS_SQUARE:		// F
				noteIndex = 8;
				break;

			case PS_CROSS:		// G
				noteIndex = 10;
				break;

			default:
				noteIndex = 12;
		}

		// User wants a sharp note
		if (control.buttons & PS_R2)
		{
			if (noteIndex != 2 && noteIndex != 7 && noteIndex != 12) // B nor E have a sharp
			{
				noteIndex++;
			}
		}

		frequency	= notes[noteIndex][0];
		note		= notes[noteIndex][1];

		// My old math made the program binary too big for the AVR.
		// The work is now based on the formula Freq = note x 2^(N/12). 
		// Note is the current note's frequency, N is how many notes
		// we want to shift up or down.
		// Fancy bit shifting allows the shoulder buttons to represent
		// a number between 0 and 7 inclusive
		octave  = (control.buttons & PS_L2) >> 8;
		octave |= (control.buttons & PS_L1) >> 9;
		octave |= (control.buttons & PS_R1) >> 9;
		frequency <<= octave;

		if (frequency != oldFrequency)
		{
			if (frequency == 0)
			{
				put(VFD_CR);
				print("Not playing note");
			}
			else
			{
				SIDSet(FREQ_HIGH, frequency >> 8);
				SIDSet(FREQ_LOW,  frequency);
				SIDSet(CONTROL,   controlRegister);
				SIDSet(CONTROL,   controlRegister | CNT_GATE);	// Gate the note (mute it)

				put(VFD_CR);
				print("Playing ");
				print(" ");
				put(note);
				put(note >> 8);	// If there's a sharp, show it
				putnum(octave);
				print("     ");
			}

			oldFrequency = frequency;
		}
	}

}
