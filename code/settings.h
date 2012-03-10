/**************************************************
	Modify SID instrument header (settings.h)
	Author:	Edwin Amsler <edwinguy@gmail.com>
	Date:	March 31, 2010
**************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include "playstation.h"
#include "mos8580.h"
#include "noritake_vfd.h"

// Not all of these configurations are settable
// yet. Need to crack open my C64 Programmer's
// Reference book to figure out the last three
// options down there...
typedef enum CONFIGURE
{
	INIT 		=  0,	// No properties
	VOLUME		=  1,	// 4  bit value
	BRIGHTNESS	=  2,	// 2  bit value
	ATTACK		=  3,	// 4  bit value
	DECAY		=  4,	// 4  bit value
	SUSTAIN		=  5,	// 4  bit value
	RELEASE		=  6,	// 4  bit value
	WAVEFORM	=  7,	// Enumeration
	PULSEWIDTH	=  8,	// 12 bit value
	SYNC		=  9,	// yes/no
	RING		= 10 	// yes/no
} CONFIGURE;

typedef enum DELTA
{
	INCREMENT,
	DECREMENT,
	NONE
} DELTA;

void playNote(unsigned short int);
void configure();

unsigned char changeValue(unsigned char, unsigned char, DELTA*);
unsigned char changeNumber(unsigned char, unsigned char, DELTA*);
unsigned char changeEnum(const char**, unsigned char, unsigned char, DELTA*);

#endif
